#include "loopconstruction.h"

void LogLoopBBs(panda::compiler::BasicBlock* header){
    ArenaVector<panda::compiler::BasicBlock *> bbs = header->GetLoop()->GetBlocks();
    std::cout << "[+] loop list >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << std::endl;
    for (size_t i = 0; i < bbs.size(); i++) {
        panda::compiler::BasicBlock * bb = bbs[i];
        std::cout << bb->GetId() << " ";
        if(bb->IsLoopValid() && bb->GetLoop()->IsRoot()){
            std::cout << "bbi@ " << bb->GetId() << std::endl;
        }
    } 

    std::cout << std::endl;
    std::cout << "[-] loop list >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << std::endl;
}

void JudgeLoopType(BasicBlock* header, std::map<compiler::Loop *, uint32_t>& loop2type, 
    std::map<compiler::Loop *, BasicBlock*> &loop2exit, 
    std::map<compiler::BasicBlock*, compiler::Loop *> &backedge2dowhileloop){

    auto &back_edges = header->GetLoop()->GetBackEdges();
    for (auto back_edge : back_edges) {
        auto succs_size = back_edge->GetSuccsBlocks().size();
        if(succs_size > 1){
            loop2type[header->GetLoop()] = 1;  // do-whle
            backedge2dowhileloop[back_edge] = header->GetLoop();

            if(back_edge->GetTrueSuccessor()->GetLoop() == header->GetLoop()){
                loop2exit[header->GetLoop()] = back_edge->GetFalseSuccessor();
            }else{
                loop2exit[header->GetLoop()] = back_edge->GetTrueSuccessor();
            }
        }else{
            loop2type[header->GetLoop()] = 0;  // while
            if(header->GetTrueSuccessor()->GetLoop() == header->GetLoop()){
                loop2exit[header->GetLoop()] = header->GetFalseSuccessor();
            }else{
                loop2exit[header->GetLoop()] = header->GetTrueSuccessor();
            }
        }
    } 

    //LogLoopBBs(header);
}

void LogBackEdgeId(ArenaVector<panda::compiler::BasicBlock *> backedges){
    std::cout << "backedgeid: ";
    for (auto it = backedges.begin(); it != backedges.end(); ++it) {
        std::cout << (*it)->GetId();
        if (std::next(it) != backedges.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

panda::compiler::BasicBlock* SearchPreHeader(panda::compiler::BasicBlock* header){
    for (auto pred : header->GetPredsBlocks()) {
        if(pred->IsLoopPreHeader()){
            return pred;
        }
    }
    return nullptr;
}