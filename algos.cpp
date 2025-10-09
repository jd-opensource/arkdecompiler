#include "algos.h"


LCAFinder::LCAFinder(panda::compiler::Graph* graph) : graph(graph) {
        Initialize();
}

panda::compiler::BasicBlock* LCAFinder::FindLCA(panda::compiler::BasicBlock* u, panda::compiler::BasicBlock* v) {
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

void LCAFinder::Initialize() {
    for (auto node : graph->GetBlocksRPO()) {
        std::unordered_set<panda::compiler::BasicBlock*> nodeAncestors;
        for (auto parent : node->GetPredsBlocks()) {
            nodeAncestors.insert(parent);
            nodeAncestors.insert(ancestors[parent].begin(), ancestors[parent].end());
        }
        ancestors[node] = nodeAncestors;
    }
}


std::vector<uint32_t> TopologicalSort(const std::vector<std::pair<uint32_t, uint32_t>>& edges) {
    if (edges.empty()) {
        return {};
    }
    
    std::unordered_set<uint32_t> allNodes;
    for (const auto& edge : edges) {
        allNodes.insert(edge.first);
        allNodes.insert(edge.second);
    }
    
    if (allNodes.empty()) {
        return {};
    }
    
    std::unordered_map<uint32_t, std::vector<uint32_t>> graph;
    std::unordered_map<uint32_t, uint32_t> indegree;
    
    for (uint32_t node : allNodes) {
        indegree[node] = 0;
    }
    
    for (const auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
        indegree[edge.second]++;
    }
    
    std::queue<uint32_t> q;
    for (uint32_t node : allNodes) {
        if (indegree[node] == 0) {
            q.push(node);
        }
    }
    
    std::vector<uint32_t> result;
    while (!q.empty()) {
        uint32_t node = q.front();
        q.pop();
        result.push_back(node);
        
        for (uint32_t neighbor : graph[node]) {
            indegree[neighbor]--;
            if (indegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }
    
    if (result.size() != allNodes.size()) {
        return {}; 
    }
    
    return result;
}