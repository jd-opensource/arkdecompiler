#include "ast.h"
#include "libpandabaseheaders.h"

class LCAFinder {
public:
    LCAFinder(panda::compiler::Graph* graph);
    panda::compiler::BasicBlock* findLCA(panda::compiler::BasicBlock* u, panda::compiler::BasicBlock* v);

private:
    panda::compiler::Graph* graph;
    std::unordered_map<panda::compiler::BasicBlock*, std::unordered_set<panda::compiler::BasicBlock*>> ancestors;

    void initialize();
};