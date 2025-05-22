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
    std::cout << "[-] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
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



void AstGen::VisitSpillFill(GraphVisitor *visitor, Inst *inst)
{
    std::cout << "[+] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    for (auto sf : inst->CastToSpillFill()->GetSpillFills()) {
        enc->set_expression_by_register(enc, sf.DstValue(), *enc->get_expression_by_register(enc, sf.SrcValue()));
    }
    std::cout << "[-] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
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
    auto dst_reg = inst->GetDstReg();
    enc->set_expression_by_register(enc, dst_reg, number);
    
    std::cout << "[-] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitIf(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIf();

    auto left_expression = *enc->get_expression_by_register(enc, inst->GetSrcReg(0));
    auto right_expression = *enc->get_expression_by_register(enc, inst->GetSrcReg(1));

    panda::es2panda::ir::Expression* test_expression;

    switch (inst->GetCc()) {
        case compiler::CC_EQ:
            test_expression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                        left_expression,
                                                        right_expression,
                                                        BinIntrinsicIdToToken(compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8));
            break;
        case compiler::CC_NE:
            test_expression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                        left_expression,
                                                        right_expression,
                                                        BinIntrinsicIdToToken(compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8));
            break;
        default:
            std::cout << "S5" << std::endl;
            UNREACHABLE();
    }

    es2panda::ir::BlockStatement* true_statements =   enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetTrueSuccessor());
    es2panda::ir::BlockStatement* false_statements =  enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetFalseSuccessor());

    auto ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);

    es2panda::ir::BlockStatement* block = enc->get_blockstatement_byid(enc, inst_base->GetBasicBlock());
    true_statements->SetParent(block);
    false_statements->SetParent(block);
    const auto &statements = block->Statements();
    block->AddStatementAtPos(statements.size(), ifStatement);

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
#endif


uint32_t onlyOneBranch(BasicBlock* father, AstGen * enc){
    auto true_branch = father->GetTrueSuccessor();
    auto false_branch = father->GetFalseSuccessor();

    // 0: if-and-else

    // 1: only if

    // 2: only else

    
    BasicBlock* analysis_block = nullptr;
    if(true_branch->GetPredsBlocks().size() == 2){
        analysis_block = true_branch;
    }else if(false_branch->GetPredsBlocks().size() == 2){
        analysis_block = false_branch;
    }else if(true_branch->GetPredsBlocks().size() == 1 && false_branch->GetPredsBlocks().size() == 1){
        return 0;
    }else{
        enc->handleError("onlyOneBranch# not consider this case");
    }

    BasicBlock* other_father = nullptr;
    if(analysis_block->GetPredecessor(0) == father){
        other_father = analysis_block->GetPredecessor(1);
    }else{
        other_father = analysis_block->GetPredecessor(0);
    }
    std::cout << "true branch: " << std::to_string(true_branch->GetId()) << std::endl;
    std::cout << "false_branch: " << std::to_string(false_branch->GetId()) << std::endl;
    std::cout << "other_fater: " << std::to_string(other_father->GetId()) << std::endl;

    while(other_father != father && other_father->GetId() !=0 ){
        other_father = other_father->GetPredecessor(0);
    }


    if(other_father == father ){
        if(analysis_block == true_branch){
            return 2;
        }else{
            return 1;
        }
    }else{
        enc->handleError("onlyOneBranch# found method is bad");

    }
    return 0;
}

void AstGen::VisitIfImm(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIfImm  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIfImm();
    auto imm = inst->GetImm();
    if (imm == 0) {
        auto source_reg = inst->GetSrcReg(0);
        auto src_expression = *enc->get_expression_by_register(enc, source_reg);
        panda::es2panda::ir::Expression* test_expression;


        auto ret = onlyOneBranch(inst->GetBasicBlock(), enc);

        es2panda::ir::BlockStatement* true_statements = nullptr;
        es2panda::ir::BlockStatement* false_statements = nullptr;
        
        if(ret == 0){
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetTrueSuccessor());
            false_statements =  enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetFalseSuccessor());
        }else if(ret == 1){
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetTrueSuccessor());
        }else{
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetFalseSuccessor());
        }

        panda::compiler::RuntimeInterface::IntrinsicId cmpid;
        switch (inst->GetCc()) {
            
            case compiler::CC_EQ:
                if(ret != 2 ){
                    cmpid = compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8;

                }else{
                    cmpid = compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8;
                }

                break;
            case compiler::CC_NE:
                if(ret != 2 ){
                    cmpid = compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8;

                }else{
                    cmpid = compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8;
                }
                break;
            default:
                std::cout << "S5" << std::endl;
                UNREACHABLE();
        }
        test_expression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                    src_expression,
                                                    enc->constant_zero,
                                                    BinIntrinsicIdToToken(cmpid));

        auto ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);

        es2panda::ir::BlockStatement* block = enc->get_blockstatement_byid(enc, inst->GetBasicBlock());

        //true_statements->SetParent(block);
        //false_statements->SetParent(block);

        const auto &statements = block->Statements();
        block->AddStatementAtPos(statements.size(), ifStatement);
    }
    std::cout << "[-] VisitIfImm  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitLoadString(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitLoadString  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToLoadString();

    std::string source_str = enc->ir_interface_->GetStringIdByOffset(inst->GetTypeId()); 
    panda::es2panda::util::StringView name_view = panda::es2panda::util::StringView(*new std::string(source_str));
    
    
   // es2panda::util::StringView literal_strview(source_str);

    auto src_expression  = AllocNode<es2panda::ir::StringLiteral>(enc, name_view);

     auto dst_reg = inst->GetDstReg();
    enc->set_expression_by_register(enc, dst_reg, src_expression);
   

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

    auto cvat = inst->CastToCastValueToAnyType();
    auto input = cvat->GetInput(0).GetInst()->CastToConstant();

    es2panda::ir::Expression* source = nullptr;
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
            source = *enc->get_expression_by_register(enc, ls_dst_reg);            
            break;
        }

        default:
           // UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" ;
            enc->success_ = false;
    }

    auto inst_dst_reg = cvat->GetDstReg();
    enc->set_expression_by_register(enc, inst_dst_reg, source);

    std::cout << "[-] VisitCastValueToAnyType  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base)
{
    std::cout << "[+] VisitIntrinsic  >>>>>>>>>>>>>>>>>" << std::endl;
    ASSERT(inst_base->IsIntrinsic());
    VisitEcma(visitor, inst_base);
    std::cout << "[-] VisitIntrinsic  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitCatchPhi(GraphVisitor *visitor, Inst *inst)
{
    std::cout << "[+] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    // The Acc register stores the exception object.
    // Create an STA instruction if the exception is used later in virtual registers.
    
    if (inst->CastToCatchPhi()->IsAcc()) {
        std::cout << "cast to catchphi" << std::endl;
        auto enc = static_cast<AstGen *>(visitor);
            enc->set_expression_by_register(enc, inst->GetDstReg(), enc->constant_catcherror);
            enc->set_expression_by_register(enc, compiler::ACC_REG_ID, enc->constant_catcherror);
        bool hasRealUsers = false;
        for (auto &user : inst->GetUsers()) {
            if (!user.GetInst()->IsSaveState()) {
                hasRealUsers = true;
                break;
            }
        }
        if (hasRealUsers) {
            
            enc->set_expression_by_register(enc, inst->GetDstReg(), enc->constant_catcherror);
            enc->set_expression_by_register(enc, compiler::ACC_REG_ID, enc->constant_catcherror);
        }
    }
    std::cout << "[-] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
}

#include "astgen_auxiins.cpp"
}  // namespace panda::bytecodeopt
