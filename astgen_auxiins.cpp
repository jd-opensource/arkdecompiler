void AstGen::VisitPhi(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    auto enc = static_cast<AstGen*>(v);
    auto inst = inst_base->CastToPhi();
    panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName(new std::string("φ"));
    ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

    for (size_t i = 0; i < inst->GetInputsCount(); i++) {
        std::cout << "[+] phi: end <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
        std::cout << "[*] reg " << std::to_string(i) << " , " << std::to_string(inst->GetSrcReg(i-2)) << std::endl;
        arguments.push_back(*enc->GetExpressionById(inst, i));
        std::cout << "[-] phi: end >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    }

    auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                        funname,
                                                        std::move(arguments),
                                                        nullptr,
                                                        false
                                                        );

    enc->SetExpressionByRegister(inst, inst->GetDstReg(), callexpression);
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

    panda::es2panda::ir::Expression* arg = nullptr;

    if(enc->memberfuncs_->find(enc->methodoffset_) != enc->memberfuncs_->end() && inst->GetArgNumber() < 3 ){
        arg = enc->GetIdentifierByName(new std::string("this"));
        
    }else{
        arg = enc->GetIdentifierByName(new std::string("arg" + std::to_string(inst->GetArgNumber()-3)));
    }
    
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

    
    if(inst->GetBasicBlock()->GetPredsBlocks().size() == 2){
        HandleError("analysis try-catch error for more than one predecessor");
    }
    
    // create null finally case
    ArenaVector<panda::es2panda::ir::Statement *> finally_statements(enc->parser_program_->Allocator()->Adapter());
    panda::es2panda::ir::BlockStatement* finnalyClause = enc->parser_program_->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(finally_statements));
    
    
    // create try-catch statement
    enc->GetBlockStatementById(inst->GetBasicBlock());

    auto tryStatement = AllocNode<panda::es2panda::ir::TryStatement>(enc, tryblock_statement, catchClause, finnalyClause);
    enc->tyridtrystatement[inst->GetBasicBlock()->GetTryId()] = tryStatement;
    
    enc->AddInstAst2BlockStatemntByInst(inst_base, tryStatement);

    std::cout << "[-] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;

}
