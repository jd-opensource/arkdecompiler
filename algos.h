#ifndef DECOMPILER_LCA
#define DECOMPILER_LCA

#include "base.h"

class LCAFinder {
public:
    LCAFinder(panda::compiler::Graph* graph);
    panda::compiler::BasicBlock* FindLCA(panda::compiler::BasicBlock* u, panda::compiler::BasicBlock* v);

private:
    panda::compiler::Graph* graph;
    std::unordered_map<panda::compiler::BasicBlock*, std::unordered_set<panda::compiler::BasicBlock*>> ancestors;

    void Initialize();
};

std::vector<uint32_t> TopologicalSort(const std::vector<std::pair<uint32_t, uint32_t>>& edges);
#endif