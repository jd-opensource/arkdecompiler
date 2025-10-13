#include "fundepscan.h"

namespace panda::bytecodeopt {

bool FunDepScan::RunImpl(){
    for (auto *bb : GetGraph()->GetBlocksLinearOrder()) {
        for (const auto &inst : bb->AllInsts()) {
            VisitInstruction(inst);
        }
    }

    this->UpdateMemberDepConstructor();
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
    switch(inst->GetIntrinsicId()){
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8:{
            auto methodoffset = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->depedges_->push_back(std::make_pair(enc->methodoffset_, methodoffset));

            auto method_name = enc->ir_interface_->GetMethodIdByOffset(methodoffset);
            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8 ||
                inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8){

                if(method_name.find("instance_initializer") != std::string::npos){
                    (*enc->class2memberfuns_)[enc->current_constructor_offset].insert(methodoffset);

                    if(enc->current_constructor_offset){
                        std::cout << "set construct2initializer: " << enc->current_constructor_offset << " : " << methodoffset << std::endl;
                        enc->construct2initializer_[enc->current_constructor_offset] = methodoffset;
                    }else{
                        HandleError("#DEFINEMETHOD: find current_constructor_offset error");
                    }
                    
                }
            }
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8:{
            auto constructor_offset = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->current_constructor_offset = constructor_offset;

            enc->inserted_construct_order_.push_back(enc->current_constructor_offset);

            // case: not include instance_initializer
            // enc->depedges_->push_back(std::make_pair(enc->methodoffset_, constructor_offset));

            auto literalarray_offset = static_cast<uint32_t>(inst->GetImms()[2]);
            auto member_functions = GetLiteralArrayByOffset(enc->program_, literalarray_offset);
            if(member_functions){
                for(auto const& member_function : *member_functions){
                    if (enc->methodname2offset_->find(member_function) != enc->methodname2offset_->end()) {
                        auto memeber_offset = (*enc->methodname2offset_)[member_function];
                        (*enc->class2memberfuns_)[constructor_offset].insert(memeber_offset);
                    }else{
                        HandleError("#function dep scan: DEFINECLASSWITHBUFFER");
                    }
                }
            }
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CREATEPRIVATEPROPERTY_PREF_IMM16_ID16:{
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto member_functions = GetLiteralArrayByOffset(enc->program_, ir_id0);
            if(member_functions){
                for(const auto& member_function : *member_functions){
                    auto memeber_offset = (*enc->methodname2offset_)[member_function];
                    (*enc->class2memberfuns_)[enc->current_constructor_offset].insert(memeber_offset);

                    // case: not include instance_initializer
                    //enc->depedges_->push_back(std::make_pair(enc->methodoffset_, memeber_offset));
                }
            }else{
                HandleError("#function dep scan: CALLRUNTIME_CREATEPRIVATEPROPERTY");
            }
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEPRIVATEPROPERTY_PREF_IMM8_IMM16_IMM16_V8:{
            auto tier = static_cast<uint32_t>(inst->GetImms()[1]);
            auto index = static_cast<uint32_t>(inst->GetImms()[2]);
            (*enc->method2lexicalmap_)[enc->methodoffset_][tier].insert(index);
            
            break;
        }

        default:;
    }
}

};
