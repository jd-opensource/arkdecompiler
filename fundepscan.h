#ifndef DECOMPILER_FUNDEPSCAN
#define DECOMPILER_FUNDEPSCAN

#include "base.h"

namespace panda::bytecodeopt {

using compiler::BasicBlock;
using compiler::Inst;
using compiler::Opcode;

// bool IsArray(const panda_file::LiteralTag &tag)
// {
//     switch (tag) {
//         case panda_file::LiteralTag::ARRAY_U1:
//         case panda_file::LiteralTag::ARRAY_U8:
//         case panda_file::LiteralTag::ARRAY_I8:
//         case panda_file::LiteralTag::ARRAY_U16:
//         case panda_file::LiteralTag::ARRAY_I16:
//         case panda_file::LiteralTag::ARRAY_U32:
//         case panda_file::LiteralTag::ARRAY_I32:
//         case panda_file::LiteralTag::ARRAY_U64:
//         case panda_file::LiteralTag::ARRAY_I64:
//         case panda_file::LiteralTag::ARRAY_F32:
//         case panda_file::LiteralTag::ARRAY_F64:
//         case panda_file::LiteralTag::ARRAY_STRING:
//             return true;
//         default:
//             return false;
//     }
// }

class FunDepScan : public compiler::Optimization, public compiler::GraphVisitor {
public:
    explicit FunDepScan(compiler::Graph *graph,  const BytecodeOptIrInterface *iface, panda::pandasm::Program* program, 
        uint32_t methodoffset, std::string fun_name, std::vector<std::pair<uint32_t, uint32_t>>* depedges)
        : compiler::Optimization(graph), ir_interface_(iface), program_(program), methodoffset_(methodoffset), fun_name_(fun_name), depedges_(depedges)
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

    bool IsArray(const panda_file::LiteralTag &tag)
    {
        switch (tag) {
            case panda_file::LiteralTag::ARRAY_U1:
            case panda_file::LiteralTag::ARRAY_U8:
            case panda_file::LiteralTag::ARRAY_I8:
            case panda_file::LiteralTag::ARRAY_U16:
            case panda_file::LiteralTag::ARRAY_I16:
            case panda_file::LiteralTag::ARRAY_U32:
            case panda_file::LiteralTag::ARRAY_I32:
            case panda_file::LiteralTag::ARRAY_U64:
            case panda_file::LiteralTag::ARRAY_I64:
            case panda_file::LiteralTag::ARRAY_F32:
            case panda_file::LiteralTag::ARRAY_F64:
            case panda_file::LiteralTag::ARRAY_STRING:
                return true;
            default:
                return false;
        }
    }

    std::optional<std::vector<std::string>> GetLiteralArrayByOffset(uint32_t offset){
        std::vector<std::string> res;
        std::stringstream hexStream;
        hexStream << "0x" << std::hex << offset;
        std::string offsetString = hexStream.str();
        
        for (const auto &[key, lit_array] :  this->program_->literalarray_table) {
            const auto &tag = lit_array.literals_[0].tag_;
                    
            if (key.find(offsetString) == std::string::npos || IsArray(tag)) {
                continue;
            }
            
            for (size_t i = 0; i < lit_array.literals_.size(); i++) {
                const auto &tag = lit_array.literals_[i].tag_;
                const auto &val = lit_array.literals_[i].value_;
                if(tag == panda_file::LiteralTag::METHOD || tag == panda_file::LiteralTag::GETTER || tag == panda_file::LiteralTag::SETTER || tag == panda_file::LiteralTag::GENERATORMETHOD){
                    res.push_back(std::get<std::string>(val));
                }
            }
            return res;
        }

        return std::nullopt;

    }

    static void VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base);
    static void VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base);

#include "compiler/optimizer/ir/visitor.inc"

    const BytecodeOptIrInterface *ir_interface_;
    [[maybe_unused]] panda::pandasm::Program* program_;
    [[maybe_unused]] uint32_t methodoffset_;
    [[maybe_unused]] std::string fun_name_;

    [[maybe_unused]] std::vector<std::pair<uint32_t, uint32_t>>* depedges_;
};

}
#endif