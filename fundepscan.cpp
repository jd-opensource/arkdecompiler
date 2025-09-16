#include "fundepscan.h"

namespace panda::bytecodeopt {

bool FunDepScan::RunImpl(){
    for (auto *bb : GetGraph()->GetBlocksLinearOrder()) {
        for (const auto &inst : bb->AllInsts()) {
            VisitInstruction(inst);
        }
    }
    return true;
}
void FunDepScan::VisitIntrinsic(GraphVisitor *visitor, Inst *inst_base)
{
    ASSERT(inst_base->IsIntrinsic());
    VisitEcma(visitor, inst_base);
}

void FunDepScan::VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base){
    
    ASSERT(inst_base->IsIntrinsic());
    auto inst = inst_base->CastToIntrinsic();
    auto enc = static_cast<FunDepScan *>(visitor);
    if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8 ||
        inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8
    ){
        auto methodoffset = static_cast<uint32_t>(inst->GetImms()[1]);
        enc->depedges_->push_back(std::make_pair(enc->methodoffset_, methodoffset));
    }else if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8 ||
        inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8
    ){
        auto constructor_offset = static_cast<uint32_t>(inst->GetImms()[1]);
        enc->depedges_->push_back(std::make_pair(enc->methodoffset_, constructor_offset));
        
        auto ir_id1 = static_cast<uint32_t>(inst->GetImms()[2]);
        auto member_functions = enc->getLiteralArrayByOffset(ir_id1);
        if(member_functions){
            std::for_each((*member_functions).begin(), (*member_functions).end(), [&enc](const std::string& word) {
                std::cout << word << std::endl;
                if (enc->methodname2offset_.find(word) != enc->methodname2offset_.end()) {
                    auto memeber_offset = enc->methodname2offset_[word];
                    enc->depedges_->push_back(std::make_pair(enc->methodoffset_, memeber_offset));
                    (*enc->class2memberfuns_)[enc->methodoffset_].push_back(memeber_offset);
                }else{
                    handleError("#function dep scan: not handle this case");
                }
            });
        }
    }
}

};