#ifndef DECOMPILER_LOOPCONSTRUCTION
#define DECOMPILER_LOOPCONSTRUCTION

#include "base.h"

using namespace panda;
using namespace bytecodeopt;

void LogLoopBBs(panda::compiler::BasicBlock* header);

void JudgeLoopType(BasicBlock* header, std::map<compiler::Loop *, uint32_t>& loop2type, 
    std::map<compiler::Loop *, BasicBlock*> &loop2exit, 
    std::map<compiler::BasicBlock*, compiler::Loop *> &backedge2dowhileloop);

void LogBackEdgeId(ArenaVector<panda::compiler::BasicBlock *> backedges);

panda::compiler::BasicBlock* SearchPreHeader(panda::compiler::BasicBlock* header);
#endif