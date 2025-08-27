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

void panda::bytecodeopt::FunDepScan::VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base){
    ASSERT(inst_base->IsIntrinsic());
    auto inst = inst_base->CastToIntrinsic();
    [[maybe_unused]] auto enc = static_cast<FunDepScan *>(visitor);
    if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8 ||
        inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8
    ){
        [[maybe_unused]] auto methodoffset = static_cast<uint32_t>(inst->GetImms()[1]);
        auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
        [[maybe_unused]] auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
    }
}

};