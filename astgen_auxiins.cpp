void AstGen::VisitPhi(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitPhi  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToPhi();
    std::cout << "[-] VisitPhi  <<<<<<<<<<<<<<<" << std::endl;
}
void AstGen::VisitTry(GraphVisitor* v, Inst* inst_base) {
    std::cout << "[+] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToTry();
    std::cout << "[-] VisitTry  >>>>>>>>>>>>>>>>>" << std::endl;
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
    pandasm::Ins ins;
    [[maybe_unused]] auto enc = static_cast<AstGen*>(v);
    [[maybe_unused]] auto inst = inst_base->CastToParameter();
    std::cout << "[-] VisitParameter  >>>>>>>>>>>>>>>>>" << std::endl;
}
