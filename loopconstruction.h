#ifndef DECOMPILER_LOOPCONSTRUCTION
#define DECOMPILER_LOOPCONSTRUCTION

#include "base.h"

namespace panda::compiler {

void LogLoopBBs(BasicBlock* header);

void JudgeLoopType(BasicBlock* header, std::map<Loop *, uint32_t>& loop2type, 
    std::map<Loop *, BasicBlock*> &loop2exit, 
    std::map<BasicBlock*, Loop *> &backedge2dowhileloop);

void LogBackEdgeId(ArenaVector<BasicBlock *> backedges);

BasicBlock* SearchPreHeader(BasicBlock* header);

}
#endif