#ifndef DECOMPILER_FUNDEPSCAN
#define DECOMPILER_FUNDEPSCAN

#include "base.h"

namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

class FunDepScan : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit FunDepScan(compiler::Graph *graph,  const BytecodeOptIrInterface *iface,  uint32_t methodoffset, std::string fun_name, 
        std::vector<std::pair<uint32_t, uint32_t>>* depedges)
        : compiler::Optimization(graph), ir_interface_(iface), methodoffset_(methodoffset), fun_name_(fun_name), depedges_(depedges)
    {

    }

    ~FunDepScan() override = default;
    bool RunImpl() override;

    const char *GetPassName() const override
    {
        return "FunDepScan";
    }

    const ArenaVector<BasicBlock *> &GetBlocksToVisit() const override
    {
        return GetGraph()->GetBlocksRPO();
    }


    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    static void VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base);

#include "compiler/optimizer/ir/visitor.inc"

    const BytecodeOptIrInterface *ir_interface_;
    [[maybe_unused]]uint32_t methodoffset_;
    [[maybe_unused]]std::string fun_name_;

    [[maybe_unused]]std::vector<std::pair<uint32_t, uint32_t>>* depedges_;
};

}
#endif