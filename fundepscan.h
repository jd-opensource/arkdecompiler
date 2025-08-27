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
        : compiler::Optimization(graph), ir_interface_(iface), methodoffset(methodoffset), fun_name(fun_name), depedges(depedges)
    {

    }

    ~FunDepScan() override = default;
    bool RunImpl() override;

    const char *GetPassName() const override
    {
        return "FunDepScan";
    }

#include "compiler/optimizer/ir/visitor.inc"
    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    void VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base);

    const BytecodeOptIrInterface *ir_interface_;
    uint32_t methodoffset;
    std::string fun_name;

    std::vector<std::pair<uint32_t, uint32_t>>* depedges;
};

}
#endif