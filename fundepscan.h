#ifndef DECOMPILER_FUNDEPSCAN
#define DECOMPILER_FUNDEPSCAN

#include "base.h"

namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

class FunDepScan : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit FunDepScan(compiler::Graph *graph,  const BytecodeOptIrInterface *iface, panda::pandasm::Program* program, 
        panda::disasm::Disassembler& disasm, uint32_t methodoffset,
        std::vector<std::pair<uint32_t, uint32_t>>* depedges,
        std::map<uint32_t, std::set<uint32_t>> *class2memberfuns,
        std::map<uint32_t, std::map<uint32_t,  std::set<size_t>>>* method2lexicalmap,
        std::set<uint32_t>* memberfuncs,
        std::map<std::string, std::string> *raw2newname,
        std::map<std::string, uint32_t> *methodname2offset,
        std::vector<uint32_t> *inserted_construct_order,
        std::map<uint32_t, uint32_t>* construct2initializer,
        std::map<uint32_t, uint32_t>* construct2staticinitializer
        )
        : compiler::Optimization(graph), ir_interface_(iface), program_(program), disasm_(disasm), methodoffset_(methodoffset), 
        depedges_(depedges), class2memberfuns_(class2memberfuns), method2lexicalmap_(method2lexicalmap),
        memberfuncs_(memberfuncs), raw2newname_(raw2newname), methodname2offset_(methodname2offset),
        inserted_construct_order_(inserted_construct_order), construct2initializer_(construct2initializer),
        construct2staticinitializer_(construct2staticinitializer)
    {
        this->current_function_initializer = 0;
        this->current_constructor_offset = 0;

        auto construct_offset = this->FindKeyWithFunction(methodoffset);
        if(construct_offset){
            this->current_constructor_offset = *construct_offset;
        }
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

    std::optional<uint32_t> FindKeyWithFunction(uint32_t functionId) const {
        for (const auto& [key, funcSet] : *class2memberfuns_) {
            if (funcSet.find(functionId) != funcSet.end()) {
                return key;
            }
        }
        return {};
    }



    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    static void VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base);

#include "compiler/optimizer/ir/visitor.inc"

    const BytecodeOptIrInterface *ir_interface_;
    panda::pandasm::Program* program_;
    [[maybe_unused]] panda::disasm::Disassembler& disasm_;
    uint32_t methodoffset_;

    std::vector<std::pair<uint32_t, uint32_t>>* depedges_; // left depends on right
    std::map<uint32_t, std::set<uint32_t>> *class2memberfuns_;

    std::map<uint32_t, std::map<uint32_t,  std::set<size_t>>> *method2lexicalmap_;

    std::set<uint32_t>* memberfuncs_;

    [[maybe_unused]] std::map<std::string, std::string> *raw2newname_;

    std::map<std::string, uint32_t> *methodname2offset_;

    std::vector<uint32_t> *inserted_construct_order_;
    std::map<uint32_t, uint32_t>* construct2initializer_;
    std::map<uint32_t, uint32_t>* construct2staticinitializer_;

    uint32_t current_constructor_offset;
    uint32_t current_function_initializer;
    uint32_t current_instance_initializer;
    uint32_t current_static_initializer;
};

}
#endif
