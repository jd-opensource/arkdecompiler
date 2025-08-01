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


void AstGen::VisitTryBegin(const compiler::BasicBlock *bb)
{
    std::cout << "[+] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
    std::cout << "[-] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
}


BasicBlock* AstGen::find_nearest_visited_pred(const std::vector<BasicBlock*>& visited, BasicBlock* block) {
    if (visited.empty()) return nullptr;
    
    ArenaVector<BasicBlock*> preds = block->GetPredsBlocks();
    if (preds.empty()) return nullptr;
    
    std::unordered_set<BasicBlock*> pred_set(preds.begin(), preds.end());
    

    for (auto it = visited.rbegin(); it != visited.rend(); ++it) {
        if (pred_set.find(*it) != pred_set.end()) {
            return *it;  
        }
    }
    
    return nullptr; 
}

bool AstGen::RunImpl()
{
    
    Reserve(function_->ins.size());
    if (!GetGraph()->GetTryBeginBlocks().empty()) {
        // Workaround for AOT and JIT
        result_.emplace_back(pandasm::Create_NOP());
    }

    std::vector<BasicBlock *> visited;

    for (auto *bb : GetGraph()->GetBlocksLinearOrder()) {
        
        if(bb->IsLoopValid() && bb->IsLoopHeader() ){
            judge_looptype(bb);
            /////////////////////////////////////////////////////////////////
            ArenaVector<panda::es2panda::ir::Statement *> statements(this->parser_program_->Allocator()->Adapter());
            auto new_block_statement = this->parser_program_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(statements));
            this->whileheader2redundant[bb] = new_block_statement;
        }

        std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ visit bbid: " << bb->GetId() << std::endl;

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        auto nearestpre = this->find_nearest_visited_pred(visited, bb);
        if(nearestpre != nullptr && this->bb2acc2expression[nearestpre] != nullptr){
            std::cout << "!!!!!!!!!!!!!!!!!!!! found pre id: " << nearestpre->GetId() << std::endl;
            this->bb2acc2expression[bb] = this->bb2acc2expression[nearestpre];
        }else{
            this->bb2acc2expression[bb] = nullptr;
        }

        visited.push_back(bb);        
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        for (const auto &inst : bb->AllInsts()) {
            [[maybe_unused]] auto start = GetResult().size();

            VisitInstruction(inst);

            if (!GetStatus()) {
                return false;
            }

            [[maybe_unused]] auto end = GetResult().size();

            ASSERT(end >= start);
        }

        // check whehter add break statement
        if(!bb->IsStartBlock()){
            BasicBlock* father = bb->GetPredecessor(0);
            if(father->IsLoopValid() && !father->GetLoop()->IsRoot()){
                if(bb->GetLoop() != father->GetLoop()  ){
                    if(bb->GetSuccsBlocks().size() == 1){
                        //std::cout << "truesucc: " << bb->GetTrueSuccessor()->GetId() << ", falsesucc: " <<  loop2exit[father->GetLoop()]->GetId() << std::endl;
                        if(bb->GetTrueSuccessor() == loop2exit[father->GetLoop()]){ 

                            this->get_blockstatement_byid(bb);
                            auto breakstatement = AllocNode<es2panda::ir::BreakStatement>(this);

                            this->add_insAst_to_blockstatemnt_by_block(bb, breakstatement);

                        }
                    }

                }
            }
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


void AstGen::VisitSpillFill(GraphVisitor *visitor, Inst *inst_base)
{
    std::cout << "[+] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    auto inst = inst_base->CastToSpillFill();

    for (auto sf : inst->GetSpillFills()) {
        if(sf.SrcType() != compiler::LocationType::REGISTER || sf.DstType() != compiler::LocationType::REGISTER ){
            handleError("VisitSpillFill # unsupoort SpillFill type");
        }
        auto it = enc->reg2expression.find(sf.SrcValue());
        if (it == enc->reg2expression.end()) {
            std::cout << "VisitSpillFill # SpillFill none register"  << std::endl; 
        }else{
            //enc->set_expression_by_register(inst, sf.DstValue(), *enc->get_expression_by_register(inst, sf.SrcValue()));
        }
    }
    std::cout << "[-] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitConstant(GraphVisitor *visitor, Inst *inst_base)
{
    std::cout << "[+] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    auto inst = inst_base->CastToConstant();
    auto type = inst->GetType();
   
    es2panda::ir::Expression* number;
    switch (type) {
        case compiler::DataType::INT64:
        case compiler::DataType::UINT64:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->GetInt64Value()
                                                        );
            break;
        case compiler::DataType::FLOAT64:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->GetDoubleValue()
                                                        );
            break;
        case compiler::DataType::INT32:
        case compiler::DataType::UINT32:
            number = AllocNode<es2panda::ir::NumberLiteral>(enc, 
                                                            inst->GetInt32Value()
                                                        );
            break;
        default:
            std::cout << "S3" << std::endl;
            UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" << type;
            enc->success_ = false;
    }

    enc->set_expression_by_register(inst, inst->GetDstReg(), number);
    
    std::cout << "[-] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitIf(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIf();

    auto left_expression = *enc->get_expression_by_id(inst, 0);
    auto right_expression = *enc->get_expression_by_id(inst, 1);

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
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    /// deal with while/do-while
    auto block = inst->GetBasicBlock();
    es2panda::ir::BlockStatement* block_statement = enc->get_blockstatement_byid(block);

    if(block->IsLoopValid() && block->IsLoopHeader()){
        std::cout << "1%%%%%%%%%%%%%%%%%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        if(enc->loop2type[block->GetLoop()] == 1){
            std::cout << "[+] do-while ===" << std::endl;

            std::cout << "[-] do-while ===" << std::endl;
        }else{
            std::cout << "[+] while ===" << std::endl;

            es2panda::ir::BlockStatement* true_statements =   enc->get_blockstatement_byid(inst->GetBasicBlock()->GetTrueSuccessor());
            es2panda::ir::BlockStatement* false_statements =  enc->get_blockstatement_byid(inst->GetBasicBlock()->GetFalseSuccessor());
    
            if(enc->loop2exit[inst->GetBasicBlock()->GetLoop() ] == inst->GetBasicBlock()->GetTrueSuccessor() ){
                std::swap(true_statements, false_statements);
            }

            auto whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                                    nullptr,
                                    test_expression, 
                                    true_statements);

            enc->add_insAst_to_blockstatemnt_by_inst(inst, whilestatement);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, false_statements);

            std::cout << "[-] while ===" << std::endl;
        }
    }else{
        std::cout << "2%%%%%%%%%%%%%%%%%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        es2panda::ir::BlockStatement* true_statements =   enc->get_blockstatement_byid(inst->GetBasicBlock()->GetTrueSuccessor());
        es2panda::ir::BlockStatement* false_statements =  enc->get_blockstatement_byid(inst->GetBasicBlock()->GetFalseSuccessor());

        auto ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);
        true_statements->SetParent(block_statement);
        false_statements->SetParent(block_statement);
        
        enc->add_insAst_to_blockstatemnt_by_inst(inst, ifStatement);

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////

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
    //std::cout << "if block: " << std::to_string(father->GetId()) << std::endl;
    auto true_branch = father->GetTrueSuccessor();
    auto false_branch = father->GetFalseSuccessor();

    // 0: if-and-else

    // 1: only if

    // 2: only else

    // loop-header === 0
    if(father->IsLoopValid() && father->IsLoopHeader()){
        return 0;
    }

    BasicBlock* analysis_block = nullptr;
    if(true_branch->GetPredsBlocks().size() == 2){
        analysis_block = true_branch;
    }else if(false_branch->GetPredsBlocks().size() == 2){
        analysis_block = false_branch;
    }else if(true_branch->GetPredsBlocks().size() == 1 && false_branch->GetPredsBlocks().size() == 1){
        return 0;
    }else{
        handleError("onlyOneBranch# not consider this case");
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
    }else if(other_father->GetId() ==0 ){
        return 0;
    }else{
        //std::cout << "end other_father: " << std::to_string(other_father->GetId()) << std::endl;
        handleError("onlyOneBranch# found method is bad");
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
        auto src_expression = *enc->get_expression_by_id(inst, 0);

        panda::es2panda::ir::Expression* test_expression;


        auto ret = onlyOneBranch(inst->GetBasicBlock(), enc);

        es2panda::ir::BlockStatement* true_statements = nullptr;
        es2panda::ir::BlockStatement* false_statements = nullptr;
        
        if(ret == 0){
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(inst->GetBasicBlock()->GetTrueSuccessor());
            false_statements =  enc->get_blockstatement_byid(inst->GetBasicBlock()->GetFalseSuccessor());
        }else if(ret == 1){
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(inst->GetBasicBlock()->GetTrueSuccessor());
        }else{
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            true_statements =   enc->get_blockstatement_byid(inst->GetBasicBlock()->GetFalseSuccessor());
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

        /////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////
        /// deal with while/do-while
        auto block = inst->GetBasicBlock();
        if(enc->backedge2dowhileloop.find(block) != enc->backedge2dowhileloop.end()){
            std::cout << "[+] do-while =====" << std::endl;
            compiler::Loop* loop = block->GetLoop();
            auto back_edges = loop->GetBackEdges();
            enc->logbackedgeid(back_edges);

            es2panda::ir::DoWhileStatement* dowhilestatement;
            if(block->GetTrueSuccessor() == loop->GetHeader()){
                dowhilestatement = AllocNode<es2panda::ir::DoWhileStatement>(enc,
                    nullptr,
                    true_statements,
                    src_expression
                );
            }else{
                dowhilestatement = AllocNode<es2panda::ir::DoWhileStatement>(enc,
                        nullptr,
                        true_statements,
                        test_expression
                        );
            }
            enc->add_insAst_to_blockstatemnt_by_block(loop->GetPreHeader(), dowhilestatement);
            enc->add_insAst_to_blockstatemnt_by_block(loop->GetPreHeader(), false_statements);
            std::cout << "[-] do-while =====" << std::endl;
        }else if(block->IsLoopValid() && block->IsLoopHeader() && enc->loop2type[block->GetLoop()] == 0 ){
            std::cout << "[+] while ===" << std::endl;
            compiler::Loop* loop = block->GetLoop();
            auto back_edges = loop->GetBackEdges();
            enc->logbackedgeid(back_edges);

            es2panda::ir::WhileStatement* whilestatement;
            if( std::find(loop->GetBlocks().begin(), loop->GetBlocks().end(), block->GetFalseSuccessor()) != loop->GetBlocks().end() ){
                if(enc->whileheader2redundant[block]->Statements().size() != 0){
                    // add redundant statement in while-header
                    enc->add_insAst_to_blockstatemnt_by_block(block->GetFalseSuccessor(), enc->whileheader2redundant[block] );
                }

                std::swap(true_statements, false_statements);
                whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                        nullptr,
                        src_expression, 
                        true_statements
                        );
            }else{
                if(enc->whileheader2redundant[block]->Statements().size() != 0){
                    // add redundant statement in while-header
                    enc->add_insAst_to_blockstatemnt_by_block(block->GetTrueSuccessor(), enc->whileheader2redundant[block] );
                }
                whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                        nullptr,
                        test_expression, 
                        true_statements
                        );
            }

            enc->add_insAst_to_blockstatemnt_by_inst(inst, whilestatement);
            enc->add_insAst_to_blockstatemnt_by_block(loop->GetPreHeader(), enc->get_blockstatement_byid(block));
            enc->add_insAst_to_blockstatemnt_by_block(loop->GetPreHeader(), false_statements);

            std::cout << "[-] while ===" << std::endl;
        }else{
            es2panda::ir::IfStatement* ifStatement;

            if(ret == 2){
                ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);
            }else{
                if(inst->GetCc() == compiler::CC_EQ){
                   std::swap(true_statements, false_statements); 
                }

                ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, src_expression, true_statements, false_statements);

            }

            enc->add_insAst_to_blockstatemnt_by_inst(inst, ifStatement);

        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        //true_statements->SetParent(block);
        //false_statements->SetParent(block);
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
    
    auto src_expression  = AllocNode<es2panda::ir::StringLiteral>(enc, name_view);

    enc->set_expression_by_register(inst, inst->GetDstReg(), src_expression);
   

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
            // auto ls_dst_reg = ls->GetDstReg();
            // source = *enc->get_expression_by_register(inst, ls_dst_reg);
            source = enc->id2expression[ls->GetId()];           
            break;
        }

        default:
           // UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" ;
            enc->success_ = false;
    }

    enc->set_expression_by_register(inst, cvat->GetDstReg(), source);

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
        enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_catcherror);
        bool hasRealUsers = false;
        for (auto &user : inst->GetUsers()) {
            if (!user.GetInst()->IsSaveState()) {
                hasRealUsers = true;
                break;
            }
        }
        if (hasRealUsers) {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_catcherror);
        }
    }
    std::cout << "[-] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
}

#include "astgen_auxiins.cpp"
}  // namespace panda::bytecodeopt
