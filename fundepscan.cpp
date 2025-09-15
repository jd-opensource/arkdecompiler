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
    [[maybe_unused]] auto enc = static_cast<FunDepScan *>(visitor);
    if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8 ||
        inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8
    ){
        std::cout << "call fundep scan success @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
        [[maybe_unused]] auto methodoffset_ = static_cast<uint32_t>(inst->GetImms()[1]);
        auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
        [[maybe_unused]] auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);

        enc->depedges_->push_back(std::make_pair(enc->methodoffset_, methodoffset_));
    }else if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8 ||
        inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8
    ){
        std::cout << "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" << std::endl;
        ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
        [[maybe_unused]] auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
        std::cout << "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222" << std::endl;
        ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
        auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
        std::cout << "33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333" << std::endl;

        auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
        ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
        
        std::cout << "44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444" << std::endl;
        auto ir_id1 = static_cast<uint32_t>(inst->GetImms()[2]);
        std::cout << "55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555" << std::endl;
        auto member_functions = enc->GetLiteralArrayByOffset(ir_id1);
        if(member_functions){
            std::for_each((*member_functions).begin(), (*member_functions).end(), [](const std::string& word) {
                std::cout << word << std::endl;
            });
        }
        std::cout << "66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666" << std::endl;
        ASSERT(inst->HasImms() && inst->GetImms().size() > 3); // NOLINTNEXTLINE(readability-container-size-empty)
        std::cout << "77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777" << std::endl;
        [[maybe_unused]] auto imm3 = static_cast<uint32_t>(inst->GetImms()[3]);
        std::cout << "88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888" << std::endl;
        [[maybe_unused]]  auto v0 = inst->GetSrcReg(0);
        std::cout << "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999" << std::endl;


    }
}

};