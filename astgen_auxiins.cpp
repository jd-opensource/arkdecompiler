void AstGen::VisitPhi(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen*>(v);
    auto inst = inst_base->CastToPhi();
    ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

    auto dst_reg_identifier = enc->GetIdentifierByReg(inst->GetId());
    enc->SetExpressionByRegister(inst, inst->GetDstReg(), dst_reg_identifier);

    for (size_t i = 0; i < inst->GetInputsCount(); i++) {
        auto bb = inst->GetPhiInputBb(i);
        auto sourceexpression = *enc->GetExpressionByRegIndex(inst, i);
        auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            dst_reg_identifier,
                                                                            sourceexpression,
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        );
        auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);

        if(std::find(enc->visited.begin(), enc->visited.end(), bb) != enc->visited.end()){
            enc->AddInstAst2BlockStatemntByBlock(bb, assignstatement);
        }else{
            if(enc->phiref2pendingredundant.find(bb) != enc->phiref2pendingredundant.end()){
                auto found_block_statement = enc->phiref2pendingredundant[bb];
                const auto &statements = found_block_statement->Statements();
                found_block_statement->AddStatementAtPos(statements.size(), assignstatement);
            }else{
                ArenaVector<panda::es2panda::ir::Statement *> statements(enc->parser_program_->Allocator()->Adapter());
                auto new_block_statement = AllocNode<es2panda::ir::BlockStatement>(enc, nullptr, std::move(statements));
                new_block_statement->AddStatementAtPos(statements.size(), assignstatement);
            }
        }
    }

    std::cout << "[-] VisitPhi  <<<<<<<<<<<<<<<" << std::endl;
}

void AstGen::VisitSaveState(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitSaveState  >>>>>>>>>>>>>>>>>" << std::endl;
    std::cout << "[-] VisitSaveState  >>>>>>>>>>>>>>>>>" << std::endl;
}
void AstGen::VisitParameter(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitParameter  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto inst = inst_base->CastToParameter();

    panda::es2panda::ir::Expression* arg = enc->getParameterName(inst->GetArgNumber());    
    enc->SetExpressionByRegister(inst, inst->GetDstReg(), arg);
    std::cout << "[-] VisitParameter  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitTry(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen*>(v);
    auto inst = inst_base->CastToTry();

    // find tryblock
    BasicBlock* tryblock = nullptr;
    if(inst->GetBasicBlock()->GetSuccessor(0)->IsCatchBegin()){
        tryblock = inst->GetBasicBlock()->GetSuccessor(1);
    }else if(inst->GetBasicBlock()->GetSuccessor(1)->IsCatchBegin()){
        tryblock = inst->GetBasicBlock()->GetSuccessor(0);
    }else{
        HandleError("can't handle this case  in visitTry for finding try block");
    }

    enc->specialblockid.insert(tryblock->GetId());
    
    panda::es2panda::ir::BlockStatement* tryblock_statement = enc->GetBlockStatementById(tryblock);

    if(inst->GetBasicBlock()->GetTryId() !=  panda::compiler::INVALID_ID){
        enc->tyrid2block[inst->GetBasicBlock()->GetTryId()] = tryblock_statement;
    }
    

    /// find case block
    auto type_ids = inst->GetCatchTypeIds();
    auto catch_indexes = inst->GetCatchEdgeIndexes();

    panda::es2panda::ir::CatchClause *catchClause = nullptr;
    for (size_t idx = 0; idx < type_ids->size(); idx++) {
        auto succ =  inst->GetBasicBlock()->GetSuccessor(catch_indexes->at(idx));
        
        while (!succ->IsCatchBegin()) {
            succ = succ->GetSuccessor(0);
        }

        enc->specialblockid.insert(succ->GetId());
        auto catch_block = enc->GetBlockStatementById(succ);
   
        panda::es2panda::ir::Expression *param = enc->constant_catcherror;
        

        catchClause =  AllocNode<panda::es2panda::ir::CatchClause>(enc, nullptr, param, catch_block);
        enc->tyrid2catchclause[inst->GetBasicBlock()->GetTryId()] = catchClause;
    }

    
    // if(inst->GetBasicBlock()->GetPredsBlocks().size() > 2){
    //     HandleError("analysis try-catch error for more than one predecessor");
    // }
    
    // create null finally case
    ArenaVector<panda::es2panda::ir::Statement *> finally_statements(enc->parser_program_->Allocator()->Adapter());
    auto finnalyClause = AllocNode<es2panda::ir::BlockStatement>(enc, nullptr, std::move(finally_statements));
    
    // create try-catch statement
    enc->GetBlockStatementById(inst->GetBasicBlock());

    auto tryStatement = AllocNode<panda::es2panda::ir::TryStatement>(enc, tryblock_statement, catchClause, finnalyClause);
    enc->tyridtrystatement[inst->GetBasicBlock()->GetTryId()] = tryStatement;
    
    enc->AddInstAst2BlockStatemntByInst(inst_base, tryStatement);

    std::cout << "[-] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;

}
