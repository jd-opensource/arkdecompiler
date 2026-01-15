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

    std::optional<uint32_t> FindKeyWithFunction(uint32_t functionId) const {
        for (const auto& [key, funcSet] : *class2memberfuns_) {
            if (funcSet.find(functionId) != funcSet.end()) {
                return key;
            }
        }
        return {};
    }

    void UpdateMemberDepConstructor(){
        // method define class > static_initializer > instance_initializer > member functions
  
        uint32_t last_class_member = 0; // multiple class analysis sequence
        for(const auto& constructor_offset : this->inserted_construct_order_){
            if(this->class2memberfuns_->find(constructor_offset) == this->class2memberfuns_->end()){
                continue;
            }
            
            auto member_funcs = (*this->class2memberfuns_)[constructor_offset];
            this->memberfuncs_->insert(constructor_offset);

            uint32_t function_initializer = 0;
            uint32_t static_function_initializer = 0;
            uint32_t predecessor_of_memeber = 0;

            if(this->construct2initializer_.find(constructor_offset) != this->construct2initializer_.end()){
                function_initializer = this->construct2initializer_[constructor_offset];
            }

            if(this->construct2staticinitializer_.find(constructor_offset) != this->construct2staticinitializer_.end()){
                static_function_initializer = this->construct2staticinitializer_[constructor_offset];
            }

            uint32_t predecessor_of_initializer;
            if(last_class_member){
                predecessor_of_initializer = last_class_member;
            }else{
                predecessor_of_initializer = this->methodoffset_;
            }

            if(static_function_initializer && function_initializer){
                this->depedges_->push_back(std::make_pair(predecessor_of_initializer, static_function_initializer));
                this->depedges_->push_back(std::make_pair(static_function_initializer, function_initializer));
                predecessor_of_memeber = function_initializer;
            }else if(static_function_initializer && !function_initializer){
                this->depedges_->push_back(std::make_pair(predecessor_of_initializer, static_function_initializer));
                predecessor_of_memeber = static_function_initializer;
            }else if(!static_function_initializer && function_initializer){
                this->depedges_->push_back(std::make_pair(predecessor_of_initializer, function_initializer));
                predecessor_of_memeber = function_initializer;
            }else{
                predecessor_of_memeber = predecessor_of_initializer;
            }

            for (const auto& memeber_offset : member_funcs) {
                this->memberfuncs_->insert(memeber_offset);
                last_class_member = memeber_offset;
                if(memeber_offset == predecessor_of_memeber){
                    continue;
                }

                if(static_function_initializer && memeber_offset == static_function_initializer ){
                    continue;
                }

                if(function_initializer && memeber_offset == function_initializer ){
                    continue;
                }

                this->depedges_->push_back(std::make_pair(predecessor_of_memeber, memeber_offset));
            }
        }
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

    std::map<uint32_t, uint32_t> construct2initializer_;
    std::map<uint32_t, uint32_t> construct2staticinitializer_;

    std::vector<uint32_t> inserted_construct_order_;

    uint32_t current_constructor_offset;
    uint32_t current_function_initializer;
    uint32_t current_instance_initializer;
    uint32_t current_static_initializer;
};

}
#endif
