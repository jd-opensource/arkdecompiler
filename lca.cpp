#include "lca.h"


LCAFinder::LCAFinder(panda::compiler::Graph* graph) : graph(graph) {
        initialize();
}

panda::compiler::BasicBlock* LCAFinder::findLCA(panda::compiler::BasicBlock* u, panda::compiler::BasicBlock* v) {
    std::unordered_set<panda::compiler::BasicBlock*> commonAncestors;
    const auto& ancestorsU = ancestors[u];
    const auto& ancestorsV = ancestors[v];

    for (auto ancestor : ancestorsU) {
        if (ancestorsV.count(ancestor)) {
            commonAncestors.insert(ancestor);
        }
    }

    panda::compiler::BasicBlock* lca = nullptr;
    int maxDepth = -1;
    for (auto ancestor : commonAncestors) {
        int depth = ancestors[ancestor].size();
        if (depth > maxDepth) {
            maxDepth = depth;
            lca = ancestor;
        }
    }
    return lca;
}

void LCAFinder::initialize() {
    for (auto node : graph->GetBlocksRPO()) {
        std::unordered_set<panda::compiler::BasicBlock*> nodeAncestors;
        for (auto parent : node->GetPredsBlocks()) {
            nodeAncestors.insert(parent);
            nodeAncestors.insert(ancestors[parent].begin(), ancestors[parent].end());
        }
        ancestors[node] = nodeAncestors;
    }
}