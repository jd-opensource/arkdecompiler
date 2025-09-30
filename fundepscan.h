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
        panda::disasm::Disassembler& disasm, uint32_t methodoffset, std::string fun_name, 
        std::vector<std::pair<uint32_t, uint32_t>>* depedges,
        std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns,
        std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>>* method2lexicalmap,
        std::vector<uint32_t>* memfuncs)
        : compiler::Optimization(graph), ir_interface_(iface), program_(program), disasm_(disasm), methodoffset_(methodoffset), 
        fun_name_(fun_name), depedges_(depedges), class2memberfuns_(class2memberfuns), method2lexicalmap_(method2lexicalmap),
        memfuncs_(memfuncs)
    {
        for (const auto& pair : this->disasm_.method_name_to_id_) {
            std::size_t pos = pair.first.find(':');
            if (pos != std::string::npos) {
                std::string result = pair.first.substr(0, pos);
                this->methodname2offset_[result] = pair.second.GetOffset();
            }
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

    void update_member_dep_constructor(){
        // Member functions are initialized after the constructor.
        for(auto const constructor_func: this->constructor_funcs_){
            for(auto const memfunc: *this->memfuncs_){
                this->depedges_->push_back(std::make_pair(constructor_func, memfunc));
            }
        }
    }

    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    static void VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base);

#include "compiler/optimizer/ir/visitor.inc"

    [[maybe_unused]] const BytecodeOptIrInterface *ir_interface_;
    [[maybe_unused]] panda::pandasm::Program* program_;
    [[maybe_unused]] panda::disasm::Disassembler& disasm_;
    [[maybe_unused]] uint32_t methodoffset_;
    [[maybe_unused]] std::string fun_name_;

    [[maybe_unused]] std::vector<std::pair<uint32_t, uint32_t>>* depedges_;
    [[maybe_unused]] std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns_;

    [[maybe_unused]] std::map<std::string, uint32_t> methodname2offset_;

    [[maybe_unused]] std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>> *method2lexicalmap_;

    [[maybe_unused]] std::vector<uint32_t> constructor_funcs_;

    [[maybe_unused]] std::vector<uint32_t>* memfuncs_;

};

}
#endif