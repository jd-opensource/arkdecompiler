#include "astgen.h"

#include "astgen_ecma.inl"

namespace panda::bytecodeopt {

using panda_file::LiteralTag;


void AstGen::VisitTryBegin(const compiler::BasicBlock *bb)
{
    std::cout << "[+] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
    std::cout << "[-] VisitTryBegin  >>>>>>>>>>>>>>>>>" << std::endl;
}


BasicBlock* AstGen::FindNearestVisitedPred(const std::vector<BasicBlock*>& visited, BasicBlock* block) {
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
    
    for (auto *bb : GetGraph()->GetBlocksRPO()) {
        std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ visit bbid: " << bb->GetId() << std::endl;
        //if(bb->IsLoopValid() && !bb->GetLoop()->IsRoot() ){
        if(bb->IsLoopValid()  ){
            auto loop = bb->GetLoop();
            auto backedges = loop->GetBackEdges();
            auto innerloop = loop->GetInnerLoops();
            auto blocks = loop->GetBlocks();
            std::cout << "Loop Size: " << backedges.size()  << " , innerloop: " << innerloop.size()  << " , block: " << blocks.size() << std::endl;

        }


        if(bb->IsLoopValid() && bb->IsLoopHeader() ){
            JudgeLoopType(bb, this->loop2type, this->loop2exit, this->backedge2dowhileloop);
            /////////////////////////////////////////////////////////////////
            ArenaVector<panda::es2panda::ir::Statement *> statements(this->parser_program_->Allocator()->Adapter());
            auto new_block_statement =  AllocNode<es2panda::ir::BlockStatement>(this, nullptr, std::move(statements));
            
            this->whileheader2redundant[bb] = new_block_statement;
        }

        

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        auto nearestpre = this->FindNearestVisitedPred(this->visited, bb);

        if(bb != this->GetGraph()->GetStartBlock()) {
            if(nearestpre != nullptr && this->bb2lexicalenvstack_[nearestpre] != nullptr){
                std::cout << "!!!!!!!!!!!!!!!!!!!! found pre id for bb2lexicalenvstack_: " << nearestpre->GetId() << std::endl;
                this->bb2lexicalenvstack_[bb] = new LexicalEnvStack(*this->bb2lexicalenvstack_[nearestpre]);
                this->bb2sendablelexicalenvstack_[bb] = new LexicalEnvStack(*this->bb2sendablelexicalenvstack_[nearestpre]);

                std::cout << "size: " << (*this->bb2lexicalenvstack_[nearestpre]).Size()  << std::endl;
                std::cout << "sendable size: " << (*this->bb2sendablelexicalenvstack_[nearestpre]).Size()  << std::endl;
            }else{
                std::cout << "!!!!!!!!!!!!!!!!!!!! not found pre id for bb2lexicalenvstack_: "<< "curid: " << bb->GetId()  << std::endl;
                this->bb2lexicalenvstack_[bb] = new LexicalEnvStack();
                this->bb2sendablelexicalenvstack_[bb] = new LexicalEnvStack();
            }
        }
        
        this->visited.push_back(bb);        
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        this->GetBlockStatementById(bb);

        for (const auto &inst : bb->AllInsts()) {
            VisitInstruction(inst);
            if (!GetStatus()) {
                return false;
            }
        }

        // check whehter add break statement
        if(!bb->IsStartBlock()){
            BasicBlock* father = bb->GetPredecessor(0);
            if(father->IsLoopValid() && !father->GetLoop()->IsRoot()){
                if(bb->GetLoop() != father->GetLoop()  ){
                    if(bb->GetSuccsBlocks().size() == 1){
                        //std::cout << "truesucc: " << bb->GetTrueSuccessor()->GetId() << ", falsesucc: " <<  loop2exit[father->GetLoop()]->GetId() << std::endl;
                        if(bb->GetTrueSuccessor() == loop2exit[father->GetLoop()]){ 

                            this->GetBlockStatementById(bb);
                            auto breakstatement = AllocNode<es2panda::ir::BreakStatement>(this);

                            this->AddInstAst2BlockStatemntByBlock(bb, breakstatement);

                        }
                    }

                }
            }
        }


        uint32_t offset = 0;
        if(bb->IsIfBlock()){
            offset = 1;
        }
        // check if add redundant block
        if(this->whilebody2redundant.find(bb) != this->whilebody2redundant.end()){
            this->inserted_statements.erase(this->whilebody2redundant[bb]);

            this->AddInstAst2BlockStatemntByBlock(bb, this->whilebody2redundant[bb], 1);
            this->whilebody2redundant.erase(bb);
        }
    
        if(this->phiref2pendingredundant.find(bb) != this->phiref2pendingredundant.end()){
            this->inserted_statements.erase(this->phiref2pendingredundant[bb]);

            this->AddInstAst2BlockStatemntByBlock(bb, this->phiref2pendingredundant[bb], 1);
            this->phiref2pendingredundant.erase(bb);
        }
        
    }

    if (!GetStatus()) {
        return false;
    }
    // Visit try-blocks in order they were declared
    for (auto *bb : GetGraph()->GetTryBeginBlocks()) {
        VisitTryBegin(bb);
    }

    return true;
}


void AstGen::VisitSpillFill(GraphVisitor *visitor, Inst *inst_base)
{
    std::cout << "[+] VisitSpillFill  >>>>>>>>>>>>>>>>>" << std::endl;
    auto *enc = static_cast<AstGen *>(visitor);
    auto inst = inst_base->CastToSpillFill();

    for (auto sf : inst->GetSpillFills()) {
        if(sf.SrcType() != compiler::LocationType::REGISTER || sf.DstType() != compiler::LocationType::REGISTER ){
            HandleError("VisitSpillFill # unsupoort SpillFill type");
        }
        auto it = enc->reg2expression.find(sf.SrcValue());
        if (it == enc->reg2expression.end()) {
            std::cout << "VisitSpillFill # SpillFill none register"  << std::endl; 
        }else{
            enc->SetExpressionByRegister(inst, sf.DstValue(), *enc->GetExpressionByRegister(inst, sf.SrcValue()));
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

    enc->SetExpressionByRegister(inst, inst->GetDstReg(), number);
    
    std::cout << "[-] VisitConstant  >>>>>>>>>>>>>>>>>" << std::endl;
}


void AstGen::VisitIf(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIf();

    auto left_expression = *enc->GetExpressionByRegIndex(inst, 0);
    auto right_expression = *enc->GetExpressionByRegIndex(inst, 1);

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
    auto block_statement = enc->GetBlockStatementById(block);

    if(block->IsLoopValid() && block->IsLoopHeader()){
        std::cout << "1%%%%%%%%%%%%%%%%%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        if(enc->loop2type[block->GetLoop()] == 1){
            std::cout << "[+] do-while ===" << std::endl;

            std::cout << "[-] do-while ===" << std::endl;
        }else{
            std::cout << "[+] while ===" << std::endl;

            auto true_statements =   enc->GetBlockStatementById(inst->GetBasicBlock()->GetTrueSuccessor());
            auto false_statements =  enc->GetBlockStatementById(inst->GetBasicBlock()->GetFalseSuccessor());
    
            if(enc->loop2exit[inst->GetBasicBlock()->GetLoop() ] == inst->GetBasicBlock()->GetTrueSuccessor() ){
                std::swap(true_statements, false_statements);
            }

            auto whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                                    nullptr,
                                    test_expression, 
                                    true_statements);

            enc->AddInstAst2BlockStatemntByInst(inst, whilestatement);
            enc->AddInstAst2BlockStatemntByInst(inst, false_statements);

            std::cout << "[-] while ===" << std::endl;
        }
    }else{
        std::cout << "2%%%%%%%%%%%%%%%%%!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        auto true_statements =   enc->GetBlockStatementById(inst->GetBasicBlock()->GetTrueSuccessor());
        auto false_statements =  enc->GetBlockStatementById(inst->GetBasicBlock()->GetFalseSuccessor());

        auto ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);
        true_statements->SetParent(block_statement);
        false_statements->SetParent(block_statement);
        
        enc->AddInstAst2BlockStatemntByInst(inst, ifStatement);

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////

    std::cout << "[-] VisitIf  >>>>>>>>>>>>>>>>>" << std::endl;
}

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
    if(true_branch->GetPredsBlocks().size() >= 2){
        analysis_block = true_branch;
    }else if(false_branch->GetPredsBlocks().size() >= 2){
        analysis_block = false_branch;
    }else if(true_branch->GetPredsBlocks().size() == 1 && false_branch->GetPredsBlocks().size() == 1){
        return 0;
    }else{
        HandleError("onlyOneBranch# not consider this case");
    }

    BasicBlock* other_father = nullptr;
    BasicBlock* start_block = father->GetGraph()->GetStartBlock();
    if(analysis_block->GetPredecessor(0) == father){
        other_father = analysis_block->GetPredecessor(1);
    }else{
        other_father = analysis_block->GetPredecessor(0);
    }
    std::cout << "true branch: " << std::to_string(true_branch->GetId()) << std::endl;
    std::cout << "false_branch: " << std::to_string(false_branch->GetId()) << std::endl;
    std::cout << "other_fater: " << std::to_string(other_father->GetId()) << std::endl;

    std::cout << "111111111111111111111111111111111111111111111111111111111" << std::endl;
    while(other_father != father && other_father != start_block){
        other_father = other_father->GetPredecessor(0);
    }
    
    std::cout << "222222222222222222222222222222222222222222222222222222222" << std::endl;
    if(other_father == father ){
        if(analysis_block == true_branch){
            return 2;
        }else{
            return 1;
        }
    }else if(other_father == start_block){
        return 0;
    }else{
        //std::cout << "end other_father: " << std::to_string(other_father->GetId()) << std::endl;
        HandleError("onlyOneBranch# found method is bad");
    }
    
    return 0;
}

panda::es2panda::ir::Expression* AstGen::InverseTestExpression(AstGen *enc, Inst* inst_base, uint32_t ret, panda::es2panda::ir::Expression* src_expression, bool swap_truefalse){
    [[maybe_unused]] auto inst = inst_base->CastToIfImm();
    auto new_src_expression = src_expression->AsBinaryExpression();
    auto raw_opeator = new_src_expression->OperatorType();
    auto new_operator = BinInverseToken2Token(raw_opeator);

    if(raw_opeator != new_operator){
        if(swap_truefalse == false && inst->GetCc() == compiler::CC_EQ){
            std::cout << "@ AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
            new_src_expression->SetOperator(new_operator);
            return new_src_expression;
        }else if(swap_truefalse == true && inst->GetCc() == compiler::CC_NE){
            std::cout << "@ BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << std::endl;
            new_src_expression->SetOperator(new_operator);
            return new_src_expression;
        }else{
            return src_expression;
        }
    }else{
        std::cout << "@ CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC" << std::endl;
        panda::compiler::RuntimeInterface::IntrinsicId cmpid;
        if(swap_truefalse == true){
            if(inst->GetCc() == compiler::CC_EQ){
                cmpid = compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8;
            }else{
                cmpid = compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8;
            }   
        }else{
            if(inst->GetCc() == compiler::CC_EQ){
                cmpid = compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8;
            }else{
                cmpid = compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8;
            }       
        }

        new_src_expression = AllocNode<es2panda::ir::BinaryExpression>(enc,
                                                    src_expression,
                                                    enc->constant_zero,
                                                    BinIntrinsicIdToToken(cmpid));
        return new_src_expression;
    }
}


void AstGen::VisitIfImm(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitIfImm  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToIfImm();
    auto imm = inst->GetImm();
    panda::es2panda::ir::Expression* test_expression;
    if (imm == 0) {
        auto src_expression = *enc->GetExpressionByRegIndex(inst, 0);

        auto ret = onlyOneBranch(inst->GetBasicBlock(), enc);

        es2panda::ir::Statement* true_statements = nullptr;
        es2panda::ir::Statement* false_statements = nullptr;
        
        if(ret == 0){
            std::cout << "#VisitIfImm ret case: " << ret << std::endl;
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            true_statements =   enc->GetBlockStatementById(inst->GetBasicBlock()->GetTrueSuccessor());
            false_statements =  enc->GetBlockStatementById(inst->GetBasicBlock()->GetFalseSuccessor());
        }else if(ret == 1){
            std::cout << "#VisitIfImm ret case: " << ret << std::endl;
            enc->specialblockid.insert(inst->GetBasicBlock()->GetTrueSuccessor()->GetId());
            true_statements =   enc->GetBlockStatementById(inst->GetBasicBlock()->GetTrueSuccessor());
        }else{
            std::cout << "#VisitIfImm ret case: " << ret << std::endl;
            enc->specialblockid.insert(inst->GetBasicBlock()->GetFalseSuccessor()->GetId());
            false_statements =   enc->GetBlockStatementById(inst->GetBasicBlock()->GetFalseSuccessor());
        }
        
        /*
            // 0: if-and-else
            // 1: only if
            // 2: only else
        */
       

        /////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////
        /// deal with while/do-while
        auto block = inst->GetBasicBlock();

        if(enc->backedge2dowhileloop.find(block) != enc->backedge2dowhileloop.end()){
            std::cout << "[+] do-while =====" << std::endl;
            compiler::Loop* loop = block->GetLoop();
            auto back_edges = loop->GetBackEdges();
            LogBackEdgeId(back_edges);
            
            es2panda::ir::DoWhileStatement* dowhilestatement;
            
            test_expression =  enc->InverseTestExpression(enc, inst_base, ret, src_expression, false);

            auto dowhilebody = enc->CopyAndCreateNewBlockStatement(true_statements);
            if(block->GetTrueSuccessor() == loop->GetHeader()){
                std::cout << "do while case 1" << std::endl;
                dowhilestatement = AllocNode<es2panda::ir::DoWhileStatement>(enc,
                    nullptr,
                    dowhilebody,
                    test_expression
                );
            }else{
                std::cout << "do while case 2" << std::endl;
                dowhilestatement = AllocNode<es2panda::ir::DoWhileStatement>(enc,
                        nullptr,
                        dowhilebody,
                        test_expression
                        );
            }

            true_statements->AsBlockStatement()->statements_.clear();
            enc->AddInstAst2BlockStatemntByBlock(block, dowhilestatement);
            enc->AddInstAst2BlockStatemntByBlock(block, false_statements);

            std::cout << "[-] do-while =====" << std::endl;
        }else if(block->IsLoopValid() && block->IsLoopHeader() && enc->loop2type[block->GetLoop()] == 0 ){
            std::cout << "[+] while ===" << std::endl;
            compiler::Loop* loop = block->GetLoop();
            auto back_edges = loop->GetBackEdges();
            LogBackEdgeId(back_edges);

            es2panda::ir::WhileStatement* whilestatement;
            if( LoopContainBlock(loop, block->GetFalseSuccessor()) ){
                std::cout << "while case 1" << std::endl;
                if(enc->whileheader2redundant[block]->Statements().size() != 0){
                    // add redundant statement in while-header

                    enc->whilebody2redundant[block->GetFalseSuccessor()] = enc->whileheader2redundant[block];
                    // adjust to RunImpl in the end of basickblock
                    // enc->AddInstAst2BlockStatemntByBlock(block->GetFalseSuccessor(), enc->whileheader2redundant[block] );
                }

                std::swap(true_statements, false_statements);
                test_expression =  enc->InverseTestExpression(enc, inst_base, ret, src_expression, true);
                whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                        nullptr,
                        test_expression, 
                        true_statements
                        );

                
            }else{
                std::cout << "while case 2" << std::endl;
                if(enc->whileheader2redundant[block]->Statements().size() != 0){
                    // add redundant statement in while-header
                    enc->whilebody2redundant[block->GetTrueSuccessor()] = enc->whileheader2redundant[block];
                }
                test_expression = enc->InverseTestExpression(enc, inst_base, ret, src_expression,false);
                whilestatement = AllocNode<es2panda::ir::WhileStatement>(enc,
                        nullptr,
                        test_expression, 
                        true_statements
                        );
            }

            if(true_statements != nullptr){
                enc->inserted_statements.insert(true_statements);
            }
            
            enc->AddInstAst2BlockStatemntByInst(inst, whilestatement);
            enc->AddInstAst2BlockStatemntByBlock(loop->GetPreHeader(), enc->GetBlockStatementById(block));
            enc->AddInstAst2BlockStatemntByInst(inst, false_statements);
            
            std::cout << "[-] while ===" << std::endl;
        }else{
            std::cout << "[+] if ===" << std::endl;
            es2panda::ir::IfStatement* ifStatement;

            if(ret == 2){
                std::cout << "if case 1" << std::endl;
                std::swap(true_statements, false_statements);
                test_expression = enc->InverseTestExpression(enc, inst_base, ret, src_expression, true);
                ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);
            }else{
                if(inst->GetCc() == compiler::CC_EQ){
                    std::cout << "if case 2" << std::endl;
                    std::swap(true_statements, false_statements);
                    test_expression = enc->InverseTestExpression(enc, inst_base, ret, src_expression, true);
                }else{
                    std::cout << "if case 3" << std::endl;
                    test_expression = enc->InverseTestExpression(enc, inst_base, ret, src_expression, false);
                }
                
                ifStatement = AllocNode<es2panda::ir::IfStatement>(enc, test_expression, true_statements, false_statements);
            }

            if(true_statements != nullptr){
                 enc->inserted_statements.insert(true_statements);
            }

            if(false_statements != nullptr){
                enc->inserted_statements.insert(false_statements);
            }
           
            enc->AddInstAst2BlockStatemntByInst(inst, ifStatement);

            std::cout << "[-] if ===" << std::endl;

        }
        /////////////////////////////////////////////////////////////////////////////////////////////////

        //true_statements->SetParent(block);
        //false_statements->SetParent(block);
    }else{
        HandleError("#VisitIfImm: unhandle case");
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

    enc->SetExpressionByRegister(inst, inst->GetDstReg(), src_expression);
   

    std::cout << "[-] VisitLoadString  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitReturn(GraphVisitor *v, Inst *inst_base)
{
    std::cout << "[+] VisitReturn  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToReturn();
    switch (inst->GetType()) {
        default:
            LOG(ERROR, BYTECODE_OPTIMIZER)
                << "Codegen for " << compiler::GetOpcodeString(inst->GetOpcode()) << " failed";
            enc->success_ = false;
    }
    std::cout << "[-] VisitReturn  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitCastValueToAnyType(GraphVisitor *visitor, Inst *inst)
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
            source = enc->id2expression[ls->GetId()];           
            break;
        }

        default:
           // UNREACHABLE();
            LOG(ERROR, BYTECODE_OPTIMIZER) << "VisitConstant with unknown type" ;
            enc->success_ = false;
    }

    enc->SetExpressionByRegister(inst, cvat->GetDstReg(), source);

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
        enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->constant_catcherror);
        bool hasRealUsers = false;
        for (auto &user : inst->GetUsers()) {
            if (!user.GetInst()->IsSaveState()) {
                hasRealUsers = true;
                break;
            }
        }
        if (hasRealUsers) {
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->constant_catcherror);
        }
    }
    std::cout << "[-] VisitCatchPhi  >>>>>>>>>>>>>>>>>" << std::endl;
}

#include "astgen_auxiins.cpp"
}  // namespace panda::bytecodeopt
