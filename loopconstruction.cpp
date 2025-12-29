#include "loopconstruction.h"

namespace panda::compiler {

void LogLoopBBs(BasicBlock* header){
    ArenaVector<BasicBlock *> bbs = header->GetLoop()->GetBlocks();
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

void JudgeLoopType(BasicBlock* header, std::map<Loop *, uint32_t>& loop2type, 
    std::map<Loop *, BasicBlock*> &loop2exit, 
    std::map<BasicBlock*, Loop *> &backedge2dowhileloop){

    std::cout << "[+] judge loop type >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    if(header->GetLoop()->IsIrreducible()){
        HandleError("IrreducibleLoop");
    }

    auto &back_edges = header->GetLoop()->GetBackEdges();
    int count = 0;
    for (auto back_edge : back_edges) {
        std::cout << "[*] " << count++ << " : " << back_edge->GetId() <<  std::endl;
        auto succs_size = back_edge->GetSuccsBlocks().size();
        if(succs_size > 1 && back_edge->IsIfBlock()){
            loop2type[header->GetLoop()] = 1;  // do-whle
            std::cout << "#JudgeLoopType : dowhile" << std::endl;
            backedge2dowhileloop[back_edge] = header->GetLoop();

            if(back_edge->GetTrueSuccessor()->GetLoop() == header->GetLoop()){
                loop2exit[header->GetLoop()] = back_edge->GetFalseSuccessor();
            }else{
                loop2exit[header->GetLoop()] = back_edge->GetTrueSuccessor();
            }
        }else{
            loop2type[header->GetLoop()] = 0;  // while
            std::cout << "#JudgeLoopType : while" << std::endl;
            if(header->GetTrueSuccessor()->GetLoop() == header->GetLoop()){
                loop2exit[header->GetLoop()] = header->GetFalseSuccessor();
            }else{
                loop2exit[header->GetLoop()] = header->GetTrueSuccessor();
            }
        }
    }

    std::cout << "[-] judge loop type >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;

    //LogLoopBBs(header);
}

bool LoopContainBlock(Loop* loop, BasicBlock *bb){
    if(std::find(loop->GetBlocks().begin(), loop->GetBlocks().end(), bb) != loop->GetBlocks().end()){
        return true;
    }

    for(auto inner_loop : loop->GetInnerLoops()){
        if(LoopContainBlock(inner_loop, bb)){
            return true;
        }
    }

    return false;
}



void LogBackEdgeId(ArenaVector<BasicBlock *> backedges){
    std::cout << "backedgeid: ";
    for (auto it = backedges.begin(); it != backedges.end(); ++it) {
        std::cout << (*it)->GetId();
        if (std::next(it) != backedges.end()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

BasicBlock* SearchPreHeader(BasicBlock* header){
    for (auto pred : header->GetPredsBlocks()) {
        if(pred->IsLoopPreHeader()){
            return pred;
        }
    }
    return nullptr;
}

}