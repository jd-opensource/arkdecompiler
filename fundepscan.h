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
        std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>>* method2lexicalmap,
        std::set<uint32_t>* memberfuncs,
        std::map<std::string, std::string> *raw2newname,
        std::map<std::string, uint32_t> *methodname2offset
        )
        : compiler::Optimization(graph), ir_interface_(iface), program_(program), disasm_(disasm), methodoffset_(methodoffset), 
        depedges_(depedges), class2memberfuns_(class2memberfuns), method2lexicalmap_(method2lexicalmap),
        memberfuncs_(memberfuncs), raw2newname_(raw2newname), methodname2offset_(methodname2offset)
    {
        this->current_function_initializer = 0;
        this->current_constructor_offset = 0;
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

    void UpdateMemberDepConstructor(){
        // method define class > instance_initializer > member functions
  
        uint32_t last_member = 0;
        for(const auto& constructor_offset : this->inserted_construct_order_){
            if(this->class2memberfuns_->find(constructor_offset) == this->class2memberfuns_->end()){
                continue;
            }

            auto member_funcs = (*this->class2memberfuns_)[constructor_offset];
            this->memberfuncs_->insert(constructor_offset);

            uint32_t function_initializer = 0;
            if(this->construct2initializer_.find(constructor_offset) != this->construct2initializer_.end()){
                function_initializer = this->construct2initializer_[constructor_offset];
            }

            if(function_initializer && last_member){
                this->depedges_->push_back(std::make_pair(last_member, function_initializer));
            }

            for (const auto& memeber_offset : member_funcs) {
                this->memberfuncs_->insert(memeber_offset);
                last_member = memeber_offset;
                if(function_initializer && function_initializer != memeber_offset){
                    this->depedges_->push_back(std::make_pair(function_initializer, memeber_offset));
                }
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

    [[maybe_unused]] std::vector<std::pair<uint32_t, uint32_t>>* depedges_; // left depends on right
    [[maybe_unused]] std::map<uint32_t, std::set<uint32_t>> *class2memberfuns_;

    [[maybe_unused]] std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>> *method2lexicalmap_;

    [[maybe_unused]] std::set<uint32_t>* memberfuncs_;

    [[maybe_unused]] std::map<std::string, std::string> *raw2newname_;

    [[maybe_unused]] std::map<std::string, uint32_t> *methodname2offset_;

    [[maybe_unused]] std::map<uint32_t, uint32_t> construct2initializer_;

    [[maybe_unused]] std::vector<uint32_t> inserted_construct_order_;

    [[maybe_unused]] uint32_t current_constructor_offset;
    [[maybe_unused]] uint32_t current_function_initializer;

};

}
#endif