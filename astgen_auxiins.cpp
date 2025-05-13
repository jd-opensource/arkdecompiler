void AstGen::VisitPhi(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToPhi();
    std::cout << "[-] VisitPhi  <<<<<<<<<<<<<<<" << std::endl;
}

void AstGen::VisitSaveState(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitSaveState  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToSaveState();
    std::cout << "[-] VisitSaveState  >>>>>>>>>>>>>>>>>" << std::endl;
}
void AstGen::VisitParameter(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitParameter  >>>>>>>>>>>>>>>>>" << std::endl;
    auto enc = static_cast<AstGen *>(v);
    auto paramInst = inst_base->CastToParameter();

    panda::es2panda::ir::Expression* arg = enc->get_identifier_byname(enc, new std::string("arg" + std::to_string(paramInst->GetArgNumber())));
    
    auto inst_dst_reg = paramInst->GetDstReg();
    enc->set_expression_by_register(enc, inst_dst_reg, arg);

    std::cout << "[-] VisitParameter  >>>>>>>>>>>>>>>>>" << std::endl;
}

void AstGen::VisitTry(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToTry();

    ArenaVector<panda::es2panda::ir::Statement *> statements1(enc->parser_program_->Allocator()->Adapter());
    panda::es2panda::ir::BlockStatement* body = enc->parser_program_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(statements1));

    if(inst->GetBasicBlock()->GetTryId() !=  panda::compiler::INVALID_ID){
        enc->tyrid2block[inst->GetBasicBlock()->GetTryId()] = body;
    }
    
    panda::es2panda::ir::Expression *param = enc->get_identifier_byname(enc, new std::string("catchexp"));;
    panda::es2panda::ir::CatchClause *catchClause = nullptr;
    /////////////////////////////////////////////////////
    auto type_ids = inst->GetCatchTypeIds();
    auto catch_indexes = inst->GetCatchEdgeIndexes();

    std::cout << "##### catchsize: " << type_ids->size() << std::endl;
    for (size_t idx = 0; idx < type_ids->size(); idx++) {
        auto succ =  inst->GetBasicBlock()->GetSuccessor(catch_indexes->at(idx));
        
        while (!succ->IsCatchBegin()) {
            succ = succ->GetSuccessor(0);
        }

        auto true_catch = succ->GetSuccessor(0);
    
        auto catch_block = enc->get_blockstatement_byid(enc, true_catch);
        catchClause =  AllocNode<panda::es2panda::ir::CatchClause>(enc, nullptr, param, catch_block);
        
    }

    /////////////////////////////////////////////////////
    if(inst->GetBasicBlock()->GetPredsBlocks().size() == 2){
        enc->handleError("analysis try-catch error for more than one predecessor");
    }

    es2panda::ir::BlockStatement* block = enc->get_blockstatement_byid(enc, inst->GetBasicBlock()->GetPredecessor(0));
    ArenaVector<panda::es2panda::ir::Statement *> statements3(enc->parser_program_->Allocator()->Adapter());
    
    panda::es2panda::ir::BlockStatement* finnalyClause = enc->parser_program_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(statements3));
    auto tryStatement = AllocNode<panda::es2panda::ir::TryStatement>(enc, body, catchClause, finnalyClause);
    body->SetParent(block);
    catchClause->SetParent(block);
    finnalyClause->SetParent(block);

    const auto &statements = block->Statements();
    block->AddStatementAtPos(statements.size(), tryStatement); 

    std::cout << "[-] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;

}
