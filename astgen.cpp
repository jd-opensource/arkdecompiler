#include "astgen.h"
#include "common.h"

#include "astgen_ecma.inl"
#include "tagged_value.h"

namespace panda::bytecodeopt {

using panda_file::LiteralTag;


void DoLda(compiler::Register reg, std::vector<pandasm::Ins> &result)
{
    if (reg != compiler::ACC_REG_ID) {
        result.emplace_back(pandasm::Create_LDA(reg));
    }
}

void DoSta(compiler::Register reg, std::vector<pandasm::Ins> &result)
{
    if (reg != compiler::ACC_REG_ID) {
        result.emplace_back(pandasm::Create_STA(reg));
    }
}

void AstGen::AppendCatchBlock(uint32_t type_id, const compiler::BasicBlock *try_begin,
                                   const compiler::BasicBlock *try_end, const compiler::BasicBlock *catch_begin,
                                   const compiler::BasicBlock *catch_end)
{
    auto cb = pandasm::Function::CatchBlock();
    if (type_id != 0) {
        cb.exception_record = ir_interface_->GetTypeIdByOffset(type_id);
    }
    cb.try_begin_label = AstGen::LabelName(try_begin->GetId());
    cb.try_end_label = "end_" + AstGen::LabelName(try_end->GetId());
    cb.catch_begin_label = AstGen::LabelName(catch_begin->GetId());
    cb.catch_end_label =
        catch_end == nullptr ? cb.catch_begin_label : "end_" + AstGen::LabelName(catch_end->GetId());
    catch_blocks_.emplace_back(cb);
}

void AstGen::VisitTryBegin(const compiler::BasicBlock *bb)
{
    std::cout << "[+] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
    ASSERT(bb->IsTryBegin());
    auto try_inst = GetTryBeginInst(bb);
    auto try_end = try_inst->GetTryEndBlock();
    ASSERT(try_end != nullptr && try_end->IsTryEnd());

    std::cout << "[-] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;

    bb->EnumerateCatchHandlers([&, bb, try_end](BasicBlock *catch_handler, size_t type_id) {
        AppendCatchBlock(type_id, bb, try_end, catch_handler);
        return true;
    });
}

bool AstGen::RunImpl()
{
    
    Reserve(function_->ins.size());
    int32_t insn_order = 0;
    if (!GetGraph()->GetTryBeginBlocks().empty()) {
        // Workaround for AOT and JIT
        result_.emplace_back(pandasm::Create_NOP());
    }

    for (auto *bb : GetGraph()->GetBlocksLinearOrder()) {
        EmitLabel(AstGen::LabelName(bb->GetId()));

        if (bb->IsTryEnd() || bb->IsCatchEnd()) {
            auto label = "end_" + AstGen::LabelName(bb->GetId());
            EmitLabel(label);
        }

        for (const auto &inst : bb->AllInsts()) {
            [[maybe_unused]] auto start = GetResult().size();

            VisitInstruction(inst);

            if (!GetStatus()) {
                return false;
            }

            [[maybe_unused]] auto end = GetResult().size();

            ASSERT(end >= start);
        }

        if (bb->NeedsJump()) {
            EmitJump(bb);
            insn_order++;
        }
        
    }

    if (!GetStatus()) {
        return false;
    }
    // Visit try-blocks in order they were declared
    for (auto *bb : GetGraph()->GetTryBeginBlocks()) {
        VisitTryBegin(bb);
    }
    function_->ins = std::move(GetResult());
    function_->catch_blocks = catch_blocks_;
    return true;
}

void AstGen::EmitJump(const BasicBlock *bb)
{
    BasicBlock *suc_bb = nullptr;
    ASSERT(bb != nullptr);

    if (bb->GetLastInst() == nullptr) {
        ASSERT(bb->IsEmpty());
        suc_bb = bb->GetSuccsBlocks()[0];
        result_.push_back(pandasm::Create_JMP(AstGen::LabelName(suc_bb->GetId())));
        return;
    }

    ASSERT(bb->GetLastInst() != nullptr);
    switch (bb->GetLastInst()->GetOpcode()) {
        case Opcode::If:
        case Opcode::IfImm:
            ASSERT(bb->GetSuccsBlocks().size() == compiler::MAX_SUCCS_NUM);
            suc_bb = bb->GetFalseSuccessor();
            break;
        default:
            suc_bb = bb->GetSuccsBlocks()[0];
            break;
    }
    result_.push_back(pandasm::Create_JMP(AstGen::LabelName(suc_bb->GetId())));
}


void AstGen::EncodeSpillFillData(const compiler::SpillFillData &sf)
{
    if (sf.SrcType() != compiler::LocationType::REGISTER || sf.DstType() != compiler::LocationType::REGISTER) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "EncodeSpillFillData with unknown move type, src_type: "
                                       << static_cast<int>(sf.SrcType())
                                       << " dst_type: " << static_cast<int>(sf.DstType());
        success_ = false;
        std::cout << "S2" << std::endl;
        UNREACHABLE();
        return;
    }
    ASSERT(sf.GetType() != compiler::DataType::NO_TYPE);
    ASSERT(sf.SrcValue() != compiler::INVALID_REG && sf.DstValue() != compiler::INVALID_REG);

    if (sf.SrcValue() == sf.DstValue()) {
        return;
    }

    pandasm::Ins move;
    result_.emplace_back(pandasm::Create_MOV(sf.DstValue(), sf.SrcValue()));
    return;
}

void AstGen::VisitSpillFill(GraphVisitor *visitor, Inst *inst)
{
    std::cout << "[+] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    for (auto sf : inst->CastToSpillFill()->GetSpillFills()) {
        enc->EncodeSpillFillData(sf);
    }
    std::cout << "[-] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
}

template <typename UnaryPred>
bool HasUserPredicate(Inst *inst, UnaryPred p)
{
    bool found = false;
    for (auto const &u : inst->GetUsers()) {
        if (p(u.GetInst())) {
            found = true;
            break;
        }
    }
    return found;
}

void AstGen::VisitConstant(GraphVisitor *visitor, Inst *inst)
{
    std::cout << "[+] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    auto type = inst->GetType();
   
    es2panda::ir::Expression* number;
    switch (type) {
        case compiler::DataType::INT64:
        case compiler::DataType::UINT64:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->CastToConstant()->GetInt64Value()
                                                        );
            break;
        case compiler::DataType::FLOAT64:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->CastToConstant()->GetDoubleValue()
                                                        );
            break;
        case compiler::DataType::INT32:
        case compiler::DataType::UINT32:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->CastToConstant()->GetInt32Value()
                                                        );
            break;
        default:
            std::cout << "S3" << std::endl;
            UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" << type;
            enc->success_ = false;
    }


    es2panda::ir::BlockStatement* block = enc->programast_->Ast();
    const auto &statements = block->Statements();

    auto dst_reg = inst->GetDstReg();

    if(dst_reg == compiler::ACC_REG_ID){
        enc->acc = number;
    }else{
        panda::es2panda::ir::Identifier* dst_reg_identifier = get_identifier(enc, dst_reg);

        auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                        dst_reg_identifier,
                                                                        number,
                                                                        es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                    );
        
        auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                            assignexpression);
        block->AddStatementAtPos(statements.size(), assignstatement);
    }
    std::cout << "[-] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::EncodeSta(compiler::Register reg, compiler::DataType::Type type)
{
    pandasm::Opcode opc;
    switch (type) {
        case compiler::DataType::ANY:
            opc = pandasm::Opcode::STA;
            break;
        default:
            std::cout << "S4" << std::endl;
            UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "EncodeSta with unknown type" << type;
            success_ = false;
    }
    pandasm::Ins sta;
    sta.opcode = opc;
    sta.regs.emplace_back(reg);

    result_.emplace_back(sta);
}

// NOLINTNEXTLINE(readability-function-size)
void AstGen::VisitIf(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIf();
    switch (inst->GetInputType(0)) {
        case compiler::DataType::ANY: {
#if defined(ENABLE_BYTECODE_OPT) && defined(PANDA_WITH_ECMASCRIPT) && defined(ARK_INTRINSIC_SET)
            IfEcma(v, inst);
            break;
#else
            [[fallthrough]];
#endif
        }
        default:
            LOG(ERROR, BYTECODE_OPTIMIZER)
                << "Codegen for " << compiler::GetOpcodeString(inst->GetOpcode()) << " failed";
            enc->success_ = false;
            break;
    }
    std::cout << "[-] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
}

#if defined(ENABLE_BYTECODE_OPT) && defined(PANDA_WITH_ECMASCRIPT)
static std::optional<coretypes::TaggedValue> IsEcmaConstTemplate(Inst const *inst)
{
    if (inst->GetOpcode() != compiler::Opcode::CastValueToAnyType) {
        return {};
    }
    auto cvat_inst = inst->CastToCastValueToAnyType();
    if (!cvat_inst->GetInput(0).GetInst()->IsConst()) {
        return {};
    }
    auto const_inst = cvat_inst->GetInput(0).GetInst()->CastToConstant();

    switch (cvat_inst->GetAnyType()) {
        case compiler::AnyBaseType::ECMASCRIPT_UNDEFINED_TYPE:
            return coretypes::TaggedValue(coretypes::TaggedValue::VALUE_UNDEFINED);
        case compiler::AnyBaseType::ECMASCRIPT_INT_TYPE:
            return coretypes::TaggedValue(static_cast<int32_t>(const_inst->GetIntValue()));
        case compiler::AnyBaseType::ECMASCRIPT_DOUBLE_TYPE:
            return coretypes::TaggedValue(const_inst->GetDoubleValue());
        case compiler::AnyBaseType::ECMASCRIPT_BOOLEAN_TYPE:
            return coretypes::TaggedValue(static_cast<bool>(const_inst->GetInt64Value() != 0));
        case compiler::AnyBaseType::ECMASCRIPT_NULL_TYPE:
            return coretypes::TaggedValue(coretypes::TaggedValue::VALUE_NULL);
        default:
            return {};
    }
}

#if defined(ARK_INTRINSIC_SET)
void AstGen::IfEcma(GraphVisitor *v, compiler::IfInst *inst)
{
    auto enc = static_cast<AstGen *>(v);

    compiler::Register reg = compiler::INVALID_REG_ID;
    coretypes::TaggedValue cmp_val;

    auto test_lhs = IsEcmaConstTemplate(inst->GetInput(0).GetInst());
    auto test_rhs = IsEcmaConstTemplate(inst->GetInput(1).GetInst());

    if (test_lhs.has_value() && test_lhs->IsBoolean()) {
        cmp_val = test_lhs.value();
        reg = inst->GetSrcReg(1);
    } else if (test_rhs.has_value() && test_rhs->IsBoolean()) {
        cmp_val = test_rhs.value();
        reg = inst->GetSrcReg(0);
    } else {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Codegen for " << compiler::GetOpcodeString(inst->GetOpcode()) << " failed";
        enc->success_ = false;
        return;
    }

    DoLda(reg, enc->result_);
    switch (inst->GetCc()) {
        case compiler::CC_EQ: {
            if (cmp_val.IsTrue()) {
                enc->result_.emplace_back(
                    pandasm::Create_ECMA_JTRUE(LabelName(inst->GetBasicBlock()->GetTrueSuccessor()->GetId())));
            } else {
                enc->result_.emplace_back(
                    pandasm::Create_ECMA_JFALSE(LabelName(inst->GetBasicBlock()->GetTrueSuccessor()->GetId())));
            }
            break;
        }
        case compiler::CC_NE: {
            if (cmp_val.IsTrue()) {
                enc->result_.emplace_back(pandasm::Create_ECMA_ISTRUE());
            } else {
                enc->result_.emplace_back(pandasm::Create_ECMA_ISFALSE());
            }
            enc->result_.emplace_back(
                pandasm::Create_ECMA_JFALSE(LabelName(inst->GetBasicBlock()->GetTrueSuccessor()->GetId())));
            break;
        }
        default:
            LOG(ERROR, BYTECODE_OPTIMIZER)
                << "Codegen for " << compiler::GetOpcodeString(inst->GetOpcode()) << " failed";
            enc->success_ = false;
            return;
    }
}
#endif
#endif

void AstGen::VisitIfImm(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIfImm  >>>>>>>>>>>>>>>>>" << std::endl;
    auto inst = inst_base->CastToIfImm();
    auto imm = inst->GetImm();
    std::cout << "[-] VisitIfImm  >>>>>>>>>>>>>>>>>" << std::endl;
    if (imm == 0) {
        IfImmZero(v, inst_base);
        return;
    }
}

void AstGen::IfImmZero(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] IfImmZero  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIfImm();
    DoLda(inst->GetSrcReg(0), enc->result_);
    auto label = LabelName(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
    std::cout << "[-] IfImmZero  >>>>>>>>>>>>>>>>>" << std::endl;
    switch (inst->GetCc()) {
        case compiler::CC_EQ:
            enc->result_.emplace_back(pandasm::Create_JEQZ(label));
            return;
        case compiler::CC_NE:
            enc->result_.emplace_back(pandasm::Create_JNEZ(label));
            return;
        default:
            std::cout << "S5" << std::endl;
            UNREACHABLE();
    }
}

void AstGen::VisitLoadString(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitLoadString  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToLoadString();
    /* Do not emit unused code for Str -> CastValueToAnyType chains */
    // if (!HasUserPredicate(inst,
    //                       [](Inst const *i) { return i->GetOpcode() != compiler::Opcode::CastValueToAnyType; })) {
    //     return;
    // }

    std::string source_str = enc->ir_interface_->GetStringIdByOffset(inst->GetTypeId()); 
    std::string* source_str_ptr = new std::string(source_str);
    panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*source_str_ptr);
    
    
    es2panda::util::StringView literal_strview(source_str);

    auto src_expression  = AllocNode<es2panda::ir::StringLiteral>(enc, name_view);

    auto dst_reg = inst->GetDstReg();
    
    if(dst_reg == compiler::ACC_REG_ID){
        enc->acc = src_expression;
    }else{
        auto dst_identifier = enc->get_identifier(enc, dst_reg);
        auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                    dst_identifier,
                                                                    src_expression,
                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                );
        auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                            assignexpression);

        es2panda::ir::BlockStatement* block = enc->programast_->Ast();
        const auto &statements = block->Statements();

        block->AddStatementAtPos(statements.size(), assignstatement);
    }
    std::cout << "[-] VisitLoadString  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitReturn(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitReturn  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToReturn();
    switch (inst->GetType()) {
        case compiler::DataType::ANY: {
#if defined(ENABLE_BYTECODE_OPT) && defined(PANDA_WITH_ECMASCRIPT)
            auto test_arg = IsEcmaConstTemplate(inst->GetInput(0).GetInst());
            if (test_arg.has_value() && test_arg->IsUndefined()) {
                enc->result_.emplace_back(pandasm::Create_RETURNUNDEFINED());
                break;
            }
#endif
            DoLda(inst->GetSrcReg(0), enc->result_);
            enc->result_.emplace_back(pandasm::Create_RETURN());
            break;
        }
        default:
            LOG(ERROR, BYTECODE_OPTIMIZER)
                << "Codegen for " << compiler::GetOpcodeString(inst->GetOpcode()) << " failed";
            enc->success_ = false;
    }
    std::cout << "[-] VisitReturn  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitCastValueToAnyType([[maybe_unused]] GraphVisitor *visitor, [[maybe_unused]] Inst *inst)
{
    std::cout << "[+] VisitCastValueToAnyType  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(visitor);
    es2panda::ir::BlockStatement* block = enc->programast_->Ast();
    const auto &statements = block->Statements();

    auto cvat = inst->CastToCastValueToAnyType();
    auto input = cvat->GetInput(0).GetInst()->CastToConstant();

    es2panda::ir::Expression* source;
    switch (cvat->GetAnyType()) {
        case compiler::AnyBaseType::ECMASCRIPT_NULL_TYPE:
            source = enc->constant_null;
            break;

        case compiler::AnyBaseType::ECMASCRIPT_UNDEFINED_TYPE: {
            source = enc->constant_undefined;
            break;
        }

        case compiler::AnyBaseType::ECMASCRIPT_INT_TYPE: {
            source = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                                input->CastToConstant()->GetIntValue()
                                            );
            break;
        }

        case compiler::AnyBaseType::ECMASCRIPT_DOUBLE_TYPE: {
            source = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                                input->CastToConstant()->GetDoubleValue()
                                                        );
            break;
        }

        case compiler::AnyBaseType::ECMASCRIPT_BOOLEAN_TYPE: {
            uint64_t val = input->GetInt64Value();
            if (val != 0) {
                source = enc->constant_true;
            } else {
                source = enc->constant_false;
            }
            break;
        }

        case compiler::AnyBaseType::ECMASCRIPT_STRING_TYPE: {
            auto ls = cvat->GetInput(0).GetInst()->CastToLoadString();
            auto ls_dst_reg = ls->GetDstReg();

            if(ls_dst_reg == compiler::ACC_REG_ID){
                source = enc->acc;
            }else{
                source = enc->get_identifier(enc, ls_dst_reg);
            }
            
            break;
        }

        default:
           // UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" ;
            enc->success_ = false;
    }

    auto inst_dst_reg = inst->GetDstReg();
    if(inst_dst_reg == compiler::ACC_REG_ID){
        enc->acc = source;
    }else{
        panda::es2panda::ir::Identifier* dst_reg_identifier = get_identifier(enc, inst_dst_reg);

        ArenaVector<es2panda::ir::VariableDeclarator *> declarators(enc->programast_->Allocator()->Adapter());
        auto *declarator = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                        dst_reg_identifier, 
                                                                        source);
                                                                        
        declarators.push_back(declarator);
        
        auto variadeclaration = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                            es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                            std::move(declarators),
                                                                            true
                                                                        );

        block->AddStatementAtPos(statements.size(), variadeclaration);
    }

    std::cout << "[-] VisitCastValueToAnyType  >>>>>>>>>>>>>>>>>" << std::endl;
}

// NOLINTNEXTLINE(readability-function-size)
void AstGen::VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base)
{
    std::cout << "[+] VisitIntrinsic  >>>>>>>>>>>>>>>>>" << std::endl;
    ASSERT(inst_base->IsIntrinsic());
    VisitEcma(visitor, inst_base);
    std::cout << "[+] VisitIntrinsic  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitCatchPhi(GraphVisitor *visitor, Inst *inst)
{
    std::cout << "[+] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    // The Acc register stores the exception object.
    // Create an STA instruction if the exception is used later in virtual registers.
    if (inst->CastToCatchPhi()->IsAcc()) {
        bool hasRealUsers = false;
        for (auto &user : inst->GetUsers()) {
            if (!user.GetInst()->IsSaveState()) {
                hasRealUsers = true;
                break;
            }
        }
        if (hasRealUsers) {
            auto enc = static_cast<AstGen *>(visitor);
            DoSta(inst->GetDstReg(), enc->result_);
        }
    }
    std::cout << "[+] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
}

#include "astgen_auxiins.cpp"
}  // namespace panda::bytecodeopt
