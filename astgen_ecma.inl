#include "intrinsicid2token.h"

void panda::bytecodeopt::AstGen::VisitEcma(panda::compiler::GraphVisitor *visitor, Inst *inst_base)
{
    ASSERT(inst_base->IsIntrinsic());
    auto inst = inst_base->CastToIntrinsic();
    auto enc = static_cast<AstGen *>(visitor);

    std::ostringstream oss;
    inst->DumpOpcode(&oss);
    std::cout << "VisitIntrinsicInst: " << oss.str() << std::endl;

    switch (inst->GetIntrinsicId()) {
       case compiler::RuntimeInterface::IntrinsicId::RETURNUNDEFINED:
       {
            auto returnstatement = AllocNode<es2panda::ir::ReturnStatement>(enc,  enc->constant_undefined);
            enc->AddInstAst2BlockStatemntByInst(inst, returnstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDINFINITY:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_infinity);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDTHIS:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_this);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDFUNCTION:
       {
            enc->HandleNewCreatedExpression(inst, enc->GetIdentifierByName(enc->fun_name_));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDGLOBAL:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_global);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDNEWTARGET:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_newtarget);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDSYMBOL:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_symbol);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::GETUNMAPPEDARGS:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_arguments);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDBIGINT_ID16:
       {
            panda::es2panda::ir::Identifier* funname = enc->GetIdentifierByName("BigInt");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            auto stroffset = static_cast<uint32_t>(inst->GetImms()[0]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            es2panda::util::StringView literal_strview(*new std::string(str));

            arguments.push_back( AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            funname,
                                                                            std::move(arguments),
                                                                            nullptr,
                                                                            false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDHOLE:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_hole);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDUNDEFINED:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_undefined);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDNULL:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_null);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTRUE:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_true);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDFALSE:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_false);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDNAN:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_nan);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::COPYRESTARGS_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::WIDE_COPYRESTARGS_PREF_IMM16:
       {
            enc->HandleNewCreatedExpression(inst, enc->constant_restargs);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::INSTANCEOF_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::ISIN_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::ADD2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::SUB2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::MUL2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::DIV2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::MOD2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::EQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::NOTEQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::LESS_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::LESSEQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::GREATER_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::GREATEREQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::SHL2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::SHR2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::ASHR2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::AND2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::OR2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::XOR2_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STRICTNOTEQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STRICTEQ_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::EXP_IMM8_V8:{
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);  
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            *enc->GetExpressionByRegIndex(inst, 0),
                                                            source_expression,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );
            enc->HandleNewCreatedExpression(inst, binexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::INC_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::DEC_IMM8:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_one,
                                                            IncDecIntrinsicIdToToken(inst->GetIntrinsicId())
            );
            enc->HandleNewCreatedExpression(inst, binexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::ISTRUE:{
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_true,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );
            enc->HandleNewCreatedExpression(inst, binexpression);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ISFALSE:{
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_false,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->HandleNewCreatedExpression(inst, binexpression);
            break;

        }

       case compiler::RuntimeInterface::IntrinsicId::NOT_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::NEG_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM16: {
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);        

            auto unaryexpression = AllocNode<es2panda::ir::UnaryExpression>(enc, 
                                                            source_expression,
                                                            UnaryPrefixIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->HandleNewCreatedExpression(inst, unaryexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::TONUMBER_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TONUMERIC_IMM8:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            panda::es2panda::ir::Identifier* funname = enc->GetIdentifierByName("Number");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(source_expression);

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            funname,
                                                                            std::move(arguments),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STGLOBALVAR_IMM16_ID16:
       case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM16_ID16:
       {
            panda::es2panda::ir::Expression* src_reg_identifier = *enc->GetExpressionByAcc(inst); 
            
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);
            auto dst_reg_identifier = enc->GetIdentifierByName(str); 
            enc->globalname2expression_[dst_reg_identifier] = src_reg_identifier;

            if(enc->not_add_assgin_for_stlexvar.find(src_reg_identifier) == enc->not_add_assgin_for_stlexvar.end()){
                auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    dst_reg_identifier,
                                                                                    src_reg_identifier,
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );
                auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
                enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

                std::cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  << " #STGLOBALVAR: " << inst->GetBasicBlock()->GetId() << std::endl;;
            }
            break;
           
        }

        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYOBJECT:
        {
            ArenaVector<es2panda::ir::Expression *> properties(enc->parser_program_->Allocator()->Adapter());
            auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                                es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                                std::move(properties),
                                                                                false
                                                                            );

            ArenaVector<es2panda::ir::VariableDeclarator *> declarators(enc->parser_program_->Allocator()->Adapter());

            enc->HandleNewCreatedExpression(inst, objectexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYARRAY_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::CREATEEMPTYARRAY_IMM8:
        {
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                            std::move(elements),
                                                                            false
                                                                            );
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators(enc->parser_program_->Allocator()->Adapter());

            enc->HandleNewCreatedExpression(inst, arrayexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM16_ID16:
        {
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto literalarray_offset = static_cast<uint32_t>(inst->GetImms()[1]);

            auto literalarray = enc->FindLiteralArrayByOffset(literalarray_offset);
            if(!literalarray){
                HandleError("get literalarray error");
            }

            /*
                std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, float, double, std::string> value_;

            */
            for (const auto& literal : literalarray->literals_) {
                if(literal.IsBoolValue()){
                    elements.push_back(AllocNode<es2panda::ir::BooleanLiteral>(enc, *new bool(std::get<bool>(literal.value_))));
                }else if(literal.IsByteValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new uint8_t(std::get<uint8_t>(literal.value_))));
                }else if(literal.IsShortValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new uint16_t(std::get<uint16_t>(literal.value_))));
                }else if(literal.IsIntegerValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new uint32_t(std::get<uint32_t>(literal.value_))));
                }else if(literal.IsLongValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new uint64_t(std::get<uint64_t>(literal.value_))));
                }else if(literal.IsFloatValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new float(std::get<float>(literal.value_))));
                }else if(literal.IsDoubleValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, *new double(std::get<double>(literal.value_))));
                }else if(literal.IsStringValue()){
                    es2panda::util::StringView literal_strview(*new std::string( std::get<std::string>(literal.value_)));

                    elements.push_back(AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview));
                }else{
                    HandleError("unsupport literal type error");
                }

                // std::visit([](const auto& value) {
                //     std::cout << "The value is: " << value << std::endl;
                // }, literal.value_);
            }

            auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                            std::move(elements),
                                                                            false
                                                                            );

            enc->HandleNewCreatedExpression(inst, arrayexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM16_ID16:
        {
            ArenaVector<es2panda::ir::Expression *> properties(enc->parser_program_->Allocator()->Adapter());
            
            auto literalarray_offset = static_cast<uint32_t>(inst->GetImms()[1]); 
            auto literalarray = enc->FindLiteralArrayByOffset(literalarray_offset);

            int count = 0;
            es2panda::ir::Expression* key;
            es2panda::ir::Expression* value;
            for (const auto& literal : literalarray->literals_) {
                es2panda::ir::Expression *tmp;
                if(literal.IsBoolValue()){
                    tmp = AllocNode<es2panda::ir::BooleanLiteral>(enc, std::get<bool>(literal.value_));
                }else if(literal.IsByteValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint8_t>(literal.value_));
                }else if(literal.IsShortValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint16_t>(literal.value_));
                }else if(literal.IsIntegerValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint32_t>(literal.value_));
                }else if(literal.IsLongValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint64_t>(literal.value_));
                }else if(literal.IsFloatValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<float>(literal.value_));
                }else if(literal.IsDoubleValue()){
                    tmp = AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<double>(literal.value_));
                }else if(literal.IsStringValue()){
                    es2panda::util::StringView literal_strview(* new std::string(std::get<std::string>(literal.value_)));
                    tmp = AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview);
                }else{
                    HandleError("unsupport literal type error");
                }

                if(count++ % 2==0){
                    key = tmp;    
                }else{
                    value = tmp;
                    properties.push_back(  AllocNode<es2panda::ir::Property>(enc, key, value) );

                }

                // std::visit([](const auto& value) {
                //     std::cout << "The value is: " << value << std::endl;
                // }, literal.value_);
            }


            auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                                es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                                std::move(properties),
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, objectexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::LDGLOBALVAR_IMM16_ID16:
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM16_ID16:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            es2panda::ir::Expression* sourceexpression = enc->GetIdentifierByName(str);

            enc->HandleNewCreatedExpression(inst, sourceexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM16_ID16:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByAcc(inst);
            
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            panda::es2panda::ir::Expression* attr_expression = enc->GetIdentifierByName(str);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);

            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
       }

       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM16_V8:
       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM8_V8:
       {
            panda::es2panda::ir::Expression* attr_expression = *enc->GetExpressionByAcc(inst);
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByRegIndex(inst, 0);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);
            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::STTOGLOBALRECORD_IMM16_ID16:
        case compiler::RuntimeInterface::IntrinsicId::STCONSTTOGLOBALRECORD_IMM16_ID16:
        {
            panda::es2panda::ir::Expression* src_expression = *enc->GetExpressionByAcc(inst);
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto dst_identifier = enc->GetIdentifierByName(str);

            enc->globalname2expression_[dst_identifier] = src_expression;

            if(enc->not_add_assgin_for_stlexvar.find(src_expression) != enc->not_add_assgin_for_stlexvar.end()){
                break;
            }
            
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators(enc->parser_program_->Allocator()->Adapter());
            auto *declarator = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                dst_identifier, 
                                                                                src_expression);
            declarators.push_back(declarator);

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::STTOGLOBALRECORD_IMM16_ID16){
                auto variadeclaration = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::CONST,
                                                                                    std::move(declarators),
                                                                                    true
                                                                                );
                enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration);
            }else{
                auto variadeclaration = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::LET,
                                                                                    std::move(declarators),
                                                                                    true
                                                                                );
                enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration);
            }

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLARG0_IMM8:
        {
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLARG1_IMM8_V8:
        {
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);


            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            auto v0 = *enc->GetExpressionByRegIndex(inst, 0);
            
            arguments.push_back(v0);
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLARGS2_IMM8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));


            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            break;

        }
       case compiler::RuntimeInterface::IntrinsicId::CALLARGS3_IMM8_V8_V8_V8:
       {
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);


            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 2));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }
       
       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS0_IMM8_V8:
       {
            
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            enc->thisptr = *enc->GetExpressionByRegIndex(inst, 0);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS1_IMM8_V8_V8:
       {
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->GetExpressionByRegIndex(inst, 0);
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS2_IMM8_V8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->GetExpressionByRegIndex(inst, 0);
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 2));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS3_IMM8_V8_V8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->GetExpressionByRegIndex(inst, 0);
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 2));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 3));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            break;
        }
       
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM8_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM16_V8_V8:
       {
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->GetExpressionByRegIndex(inst, 0),
                                                        *enc->GetExpressionByRegIndex(inst, 1), 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);
            panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;

        }


       case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM16_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM8_ID16_V8:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->GetExpressionByRegIndex(inst, 0),
                                                        enc->GetIdentifierByName(str),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);


            panda::es2panda::ir::Expression* assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
                                                                        
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM16_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWOBJRANGE_PREF_IMM16_V8:
       {
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::Expression *callee = *enc->GetExpressionByRegIndex(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_NEWOBJRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            for (uint32_t i = 1; i < argsum; ++i) {
                arguments.push_back(*enc->GetExpressionByRegIndex(inst, i));
            }

            es2panda::ir::Expression *newExprNode = AllocNode<es2panda::ir::NewExpression>(enc, callee, nullptr, std::move(arguments));

            enc->HandleNewCreatedExpression(inst, newExprNode);
            break;

        }

       case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM16_V8:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto arrayexpression = source_expression->AsArrayExpression();
            es2panda::ir::Expression *callee = *enc->GetExpressionByRegIndex(inst, 0);


            auto constElements = arrayexpression->Elements();
            auto &nonConstElements = const_cast<ArenaVector<es2panda::ir::Expression *> &>(constElements);

            es2panda::ir::Expression *newExprNode = AllocNode<es2panda::ir::NewExpression>(enc, callee, nullptr, std::move(nonConstElements));
            enc->HandleNewCreatedExpression(inst, newExprNode);
            
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::COPYDATAPROPERTIES_V8:
       {
            auto src_obj = *enc->GetExpressionByAcc(inst);
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto v0 = inst->GetSrcReg(0);

            auto target_obj = *enc->GetExpressionByRegIndex(inst, 0);
            auto target_objexpression = target_obj->AsObjectExpression();

            auto target_properties = target_objexpression->Properties();
            for (auto *it : target_properties) {
                elements.push_back(it);
            }

            elements.push_back( AllocNode<es2panda::ir::SpreadElement>(enc, es2panda::ir::AstNodeType::SPREAD_ELEMENT, src_obj));
            auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                                es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                                std::move(elements),
                                                                                false
                                                                            );

            enc->SetExpressionByRegister(inst, inst->GetDstReg(), objectexpression);
            enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), v0, objectexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::APPLY_IMM8_V8_V8:
       {
            auto fun = *enc->GetExpressionByAcc(inst);
            auto raw_expression = *enc->GetExpressionByRegIndex(inst, 1);

            auto raw_array_expression = raw_expression->AsArrayExpression();

            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());

            for (auto *it :raw_array_expression->Elements()) {
                elements.push_back(it);
            }

            auto this_expression = *enc->GetExpressionByRegIndex(inst, 0);
            enc->thisptr = this_expression;
            
            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            fun,
                                                                            std::move(elements),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STARRAYSPREAD_V8_V8:
       {
            auto element = *enc->GetExpressionByAcc(inst);
            es2panda::ir::Expression* spreadelement = AllocNode<es2panda::ir::SpreadElement>(enc, es2panda::ir::AstNodeType::SPREAD_ELEMENT, element);
            auto raw_obj = *enc->GetExpressionByRegIndex(inst, 0);

            if(!raw_obj->IsArrayExpression()){
                std::cout << "###: " << std::to_string(static_cast<int>(raw_obj->Type())) << std::endl;
                HandleError("#STARRAYSPREAD: cann't deal expression except ArrayExpression");
            }

            auto raw_arrayexpression = raw_obj->AsArrayExpression();
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto index_expression = *enc->GetExpressionByRegIndex(inst, 1);
            auto index_literal = index_expression->AsNumberLiteral();
            uint32_t index = index_literal->Number();

            uint32_t count = 0;
            bool inserted = false;
            for (auto *it : raw_arrayexpression->Elements()) {
                if(count == index){
                    elements.push_back(spreadelement);
                    inserted = true;
                }
                elements.push_back(it);
                count++;
            }

            if(!inserted){
                if(index == (raw_arrayexpression->Elements().size() ) ){
                    elements.push_back(spreadelement);
                }else{
                    std::cout << "element size: " << raw_arrayexpression->Elements().size() << " , index: " << index << std::endl;
                    HandleError("#STARRAYSPREAD inset element error");
                }
            }

            auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                            std::move(elements),
                                                                            false
                                                                            );

            enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), arrayexpression);
            uint32_t size = elements.size();
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->GetLiteralByNum(size));


            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::DEBUGGER:
       {
            auto debuggerstatement = AllocNode<es2panda::ir::DebuggerStatement>(enc );
            enc->AddInstAst2BlockStatemntByInst(inst, debuggerstatement);
            break;
       }

       case compiler::RuntimeInterface::IntrinsicId::CALLRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLRANGE_PREF_IMM16_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_CALLRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 0; i < argsum; ++i) {
                arguments.push_back(*enc->GetExpressionByRegIndex(inst, i));
            }

            
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLTHISRANGE_PREF_IMM16_V8:
        case compiler::RuntimeInterface::IntrinsicId::CALLTHISRANGE_IMM8_IMM8_V8:
        {
            panda::es2panda::ir::Expression* funname = *enc->GetExpressionByAcc(inst);
            enc->thisptr =  *enc->GetExpressionByRegIndex(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_CALLTHISRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 1; i <= argsum; ++i) {
                arguments.push_back(*enc->GetExpressionByRegIndex(inst, i));
            }

            
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLTHISRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8:
       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLARROWRANGE_IMM8_IMM8_V8:  // array function hard trigger
       case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLARROWRANGE_PREF_IMM16_V8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("super");
            enc->thisptr =  *enc->GetExpressionByRegIndex(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8 ||
                inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLARROWRANGE_PREF_IMM16_V8
            ){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 1; i <= argsum; ++i) {
                arguments.push_back(*enc->GetExpressionByRegIndex(inst, i-1));
            }
            
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DELOBJPROP_V8:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByRegIndex(inst, 0);
            panda::es2panda::ir::Expression* attr_expression = *enc->GetExpressionByAcc(inst);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);



            auto unaryexpression = AllocNode<es2panda::ir::UnaryExpression>(enc, 
                                                            objattrexpression,
                                                            UnaryPrefixIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->HandleNewCreatedExpression(inst, unaryexpression);
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISFALSE_PREF_IMM8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("runtime.isfalse");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISTRUE_PREF_IMM8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("runtime.istrue");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(*enc->GetExpressionByAcc(inst));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::RETURN:
        {
            auto returnexpression = *enc->GetExpressionByAcc(inst); 
            auto returnstatement = AllocNode<es2panda::ir::ReturnStatement>(enc,  returnexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, returnstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::THROW_PREF_NONE:
       {
            auto argument = *enc->GetExpressionByAcc(inst);

            auto throwStatement = AllocNode<es2panda::ir::ThrowStatement>(enc, argument);
            enc->AddInstAst2BlockStatemntByInst(inst, throwStatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM16_V8_V8: 
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM8_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM16_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM8_V8_V8:
       {
            panda::es2panda::ir::Identifier* value_reg_identifier = enc->GetIdentifierByReg(inst->GetSrcReg(1));
            /////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto raw_obj = *enc->GetExpressionByRegIndex(inst, 0);
            if(raw_obj->IsObjectExpression()){
                auto expression2 = *enc->GetExpressionByRegIndex(inst, 1);

                enc->SetExpressionByRegister(inst->GetInput(1).GetInst(), inst->GetSrcReg(0), value_reg_identifier);

                ArenaVector<es2panda::ir::VariableDeclarator *> declarators2(enc->parser_program_->Allocator()->Adapter());
                auto *declarator2 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                value_reg_identifier, 
                                                                                expression2
                                                                            );
                declarators2.push_back(declarator2);
                auto variadeclaration2 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                    std::move(declarators2),
                                                                                    true);
                enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration2);
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                
                auto objexpression = raw_obj->AsObjectExpression();
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                ArenaVector<es2panda::ir::Expression *> new_properties(enc->parser_program_->Allocator()->Adapter());
                for (auto *it : objexpression->Properties()) {
                    new_properties.push_back(it);
                }
                new_properties.push_back(  AllocNode<es2panda::ir::Property>(enc, value_reg_identifier, *enc->GetExpressionByAcc(inst)) );
                auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                                    es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                                    std::move(new_properties),
                                                                                    false
                                                                                );
                enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), objectexpression);

            }else if(raw_obj->IsArrayExpression()){
                auto raw_arrayexpression = raw_obj->AsArrayExpression();
                ArenaVector<es2panda::ir::Expression *> new_elements(enc->parser_program_->Allocator()->Adapter());
                
                for (auto *it : raw_arrayexpression->Elements()) {
                    new_elements.push_back(it);
                }
                new_elements.push_back(*enc->GetExpressionByAcc(inst));
                auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                                es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                                std::move(new_elements),
                                                                                false
                                                                                );
                enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), arrayexpression);
            }else{
                std::cout << "###: " << std::to_string(static_cast<int>(raw_obj->Type())) << std::endl;
                HandleError("#STARRAYSPREAD: cann't deal expression except ObjectExpression");
            }



            // panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByReg(inst->GetSrcReg(0));
            // auto expression1 = *enc->GetExpressionByRegIndex(inst, 0);

            // enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), obj_reg_identifier);
            // ArenaVector<es2panda::ir::VariableDeclarator *> declarators1(enc->parser_program_->Allocator()->Adapter());
            // auto *declarator1 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
            //                                                                 obj_reg_identifier, 
            //                                                                 expression1);
            // declarators1.push_back(declarator1);
            // auto variadeclaration1 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
            //                                                                       es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
            //                                                                       std::move(declarators1),
            //                                                                       true
            //                                                                     );
            // enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration1);
            
            
            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // panda::es2panda::ir::Identifier* index_reg_identifier = enc->GetIdentifierByReg(inst->GetSrcReg(1));
            // auto expression2 = *enc->GetExpressionByRegIndex(inst, 1);

            // enc->SetExpressionByRegister(inst->GetInput(1).GetInst(), inst->GetSrcReg(0), index_reg_identifier);

            // ArenaVector<es2panda::ir::VariableDeclarator *> declarators2(enc->parser_program_->Allocator()->Adapter());
            // auto *declarator2 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
            //                                                                 index_reg_identifier, 
            //                                                                 expression2
            //                                                             );
            // declarators2.push_back(declarator2);
            // auto variadeclaration2 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
            //                                                                       es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
            //                                                                       std::move(declarators2),
            //                                                                       true
            //                                                                     );
            // enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration2);

            // ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
            //                                                                     obj_reg_identifier,
            //                                                                     index_reg_identifier, 
            //                                                                     es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
            //                                                                     true, 
            //                                                                     false
            //                                                                 );

            // auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
            //                                                                       objattrexpression,
            //                                                                       *enc->GetExpressionByAcc(inst),
            //                                                                        es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
            //                                                                 );

            // auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            // enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM8_V8_IMM16: 
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM16_V8_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STOWNBYINDEX_PREF_V8_IMM32:
       {
            uint32_t index;
            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_STOWNBYINDEX_PREF_V8_IMM32){
                index = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                index = static_cast<uint32_t>(inst->GetImms()[1]);
            }
            auto raw_obj = *enc->GetExpressionByRegIndex(inst, 0);
            if(!raw_obj->IsArrayExpression()){
                std::cout << "###: " << std::to_string(static_cast<int>(raw_obj->Type())) << std::endl;
                HandleError("#STARRAYSPREAD: cann't deal expression except ArrayExpression");
            }
            auto raw_arrayexpression = raw_obj->AsArrayExpression();
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());

            uint32_t count = 0;
            bool inserted = false;
            for (auto *it :raw_arrayexpression->Elements()) {
                if(count++ == index){
                    inserted = true;
                    elements.push_back(*enc->GetExpressionByAcc(inst));
                }
                elements.push_back(it);
            }

            if(!inserted && index == raw_arrayexpression->Elements().size()){
                elements.push_back(*enc->GetExpressionByAcc(inst));
            }else{
                HandleError("#STARRAYSPREAD: insert over array end");
            }


            [[maybe_unused]]auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                            std::move(elements),
                                                                            false
                                                                            );
            
            enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), arrayexpression);
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM16_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM16_ID16_V8:
       {
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByReg(inst->GetSrcReg(0));
            auto expression1 = *enc->GetExpressionByRegIndex(inst, 0);

            enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), obj_reg_identifier);
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators1(enc->parser_program_->Allocator()->Adapter());
            auto *declarator1 = AllocNode<es2panda::ir::VariableDeclarator>(enc, obj_reg_identifier, expression1);
            declarators1.push_back(declarator1);
            auto variadeclaration1 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                  es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                  std::move(declarators1),
                                                                                  true
                                                                                );
            enc->AddInstAst2BlockStatemntByInst(inst, variadeclaration1);
            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_reg_identifier,
                                                        enc->GetIdentifierByName(str),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);   
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                 objattrexpression,
                                                                                 *enc->GetExpressionByAcc(inst),
                                                                                 es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        );

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            break;
        }
 
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8:
        {
            std::cout << "define function >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            auto method_offset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto method_name = enc->ir_interface_->GetMethodIdByOffset(method_offset);
  
            CopyLexicalenvStack(method_offset, inst, enc->method2lexicalenvstack_, enc->bb2lexicalenvstack_, enc->globallexical_waitlist_);
            CopyLexicalenvStack(method_offset, inst, enc->method2sendablelexicalenvstack_, enc->bb2sendablelexicalenvstack_, enc->globalsendablelexical_waitlist_);

            //std::string newname = enc->RemovePrefixOfFunc(method_name);
            auto new_expression = enc->GetIdentifierByName(method_name);

            // support callruntime.createprivateproperty
            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8 ||
                inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8){

                if(method_name.find("instance_initializer") != std::string::npos){
                    MergeMethod2LexicalMap(inst, enc->bb2lexicalenvstack_, method_offset, enc->methodoffset_, enc->method2lexicalmap_);
                    
                }
            }

            enc->not_add_assgin_for_stlexvar.insert(new_expression);
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), new_expression);

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENVWITHNAME_PREF_IMM16_ID16:// current no use default name
        case compiler::RuntimeInterface::IntrinsicId::NEWLEXENVWITHNAME_IMM8_ID16: // current no use default name
        case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENV_PREF_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::NEWLEXENV_IMM8:
        {
            auto lexenv_size = static_cast<uint32_t>(inst->GetImms()[0]);
            std::cout << "lexenv_size: " << lexenv_size << std::endl;
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            std::cout << "size: " << lexicalenvstack->Size() << std::endl; 
            lexicalenvstack->Push(lexenv_size);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM4_IMM4:
       case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM8_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_STLEXVAR_PREF_IMM16_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM4_IMM4:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM8_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDESTSENDABLEVAR_PREF_IMM16_IMM16:
       {
            std::cout << "@@@ stlevar >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            std::cout << enc->methodoffset_ << std::endl;

            auto tier = static_cast<uint32_t>(inst->GetImms()[0]);
            auto index = static_cast<uint32_t>(inst->GetImms()[1]);

            std::cout << "tier: " << std::to_string(tier) << ", index: " << std::to_string(index) << std::endl;

            LexicalEnvStack* lexicalenvstack;
            if(inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM4_IMM4 || 
                inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM8_IMM8 || 
                inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::WIDE_STLEXVAR_PREF_IMM16_IMM16){
                lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            }else{
                lexicalenvstack = enc->bb2sendablelexicalenvstack_[inst->GetBasicBlock()];
            }
            
            std::cout << "size: " << lexicalenvstack->Size() << std::endl; 
            std::cout << "env size: " << lexicalenvstack->GetLexicalEnv(0).Size() << std::endl;

            auto raw_expression  = *enc->GetExpressionByAcc(inst);
            std::string closure_name;
            
            if(enc->not_add_assgin_for_stlexvar.find(raw_expression) == enc->not_add_assgin_for_stlexvar.end()){
                closure_name =  "closure_" + std::to_string(enc->methodoffset_) + "_" + std::to_string(enc->closure_count);
                enc->closure_count++;

                auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                      enc->GetIdentifierByName(closure_name),
                                                                                      raw_expression,
                                                                                      es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            ); 
                auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
                enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
                lexicalenvstack->Set(tier, index, new std::string(closure_name));
            }else{
                auto idname = enc->GetNameFromExpression(raw_expression);
                if(idname){
                    closure_name = *idname;
                }else{
                    HandleError("#STLEXVAR: not deal this case for find expression string name");
                }
                lexicalenvstack->Set(tier, index, new std::string(*idname));
            }


            std::cout << "size: " << lexicalenvstack->Size() << std::endl;
            std::cout << "env size: " << lexicalenvstack->GetLexicalEnv(0).Size() << std::endl;

            ////////////////////////////////////////////////////////////////////////////////
            /// support forward reference stack
            if(inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM4_IMM4 
                || inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM8_IMM8 
                || inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::WIDE_STLEXVAR_PREF_IMM16_IMM16){
                DealWithGlobalLexicalWaitlist(tier, index, closure_name, enc->globallexical_waitlist_);
            }else{
                DealWithGlobalLexicalWaitlist(tier, index, closure_name, enc->globalsendablelexical_waitlist_);
            }
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM4_IMM4:
        case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM8_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLEXVAR_PREF_IMM16_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM4_IMM4:
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM8_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEVAR_PREF_IMM16_IMM16:
       {
            std::cout << "@@@ ldlexvar >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            std::cout << enc->methodoffset_ << std::endl;
            auto tier = static_cast<uint32_t>(inst->GetImms()[0]);
            auto index = static_cast<uint32_t>(inst->GetImms()[1]);

            std::cout << "tier: " << std::to_string(tier) << ", index: " << std::to_string(index) << std::endl;

            LexicalEnvStack* lexicalenvstack;
            if(inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM4_IMM4 || 
                inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM8_IMM8 || 
                inst->GetIntrinsicId() ==  compiler::RuntimeInterface::IntrinsicId::WIDE_LDLEXVAR_PREF_IMM16_IMM16){

                lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            }else{

                lexicalenvstack = enc->bb2sendablelexicalenvstack_[inst->GetBasicBlock()];
            }
            
            std::cout << "size: " << lexicalenvstack->Size() << std::endl;

            if(lexicalenvstack->GetLexicalEnv(tier)[index] == nullptr){
                HandleError("#LDLEXVAR: lexicalenv is null");
            }

            auto identifier_name = lexicalenvstack->Get(tier, index);
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->GetIdentifierByName(identifier_name));

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::POPLEXENV:
        {
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            lexicalenvstack->Pop();
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STMODULEVAR_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_STMODULEVAR_PREF_IMM16:
       {
            auto moudlevar_offset = static_cast<uint32_t>(inst->GetImms()[0]);
            if(moudlevar_offset > enc->localnamespaces_.size()){
                HandleError("#STMODULEVAR moudlevar_offset not in localnamespaces_");
            }

            auto moudlevar_rawname = enc->localnamespaces_[moudlevar_offset];
            auto source_expression = enc->GetExpressionByAcc(inst); 
            if(source_expression){
                panda::es2panda::ir::Identifier* moudlevar = enc->GetIdentifierByName(moudlevar_rawname);
                auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    moudlevar,
                                                                                    *source_expression,
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );
                auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
                enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDLOCALMODULEVAR_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::LDEXTERNALMODULEVAR_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLOCALMODULEVAR_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDEXTERNALMODULEVAR_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYMODULEVAR_PREF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYMODULEVAR_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEEXTERNALMODULEVAR_PREF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEEXTERNALMODULEVAR_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYSENDABLEMODULEVAR_PREF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYSENDABLEMODULEVAR_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLELOCALMODULEVAR_PREF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLELOCALMODULEVAR_PREF_IMM16:
       {
            auto moudlevar_offset = static_cast<uint32_t>(inst->GetImms()[0]);
            if(moudlevar_offset < enc->localnamespaces_.size()){
                auto moudlevar_rawname = enc->localnamespaces_[moudlevar_offset];
                panda::es2panda::ir::Identifier* moudlevar = enc->GetIdentifierByName(moudlevar_rawname);
                enc->SetExpressionByRegister(inst, inst->GetDstReg(), moudlevar);
            }else{
                HandleError("#LDLOCALMODULEVAR: module slot not in localnamespaces_");
            }

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::TESTIN_IMM8_IMM16_IMM16:
       {
            std::cout << enc->methodoffset_ << std::endl;
            auto tier = static_cast<uint32_t>(inst->GetImms()[0]);
            auto index = static_cast<uint32_t>(inst->GetImms()[1]);

            std::cout << "tier: " << std::to_string(tier) << ", index: " << std::to_string(index) << std::endl;
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            std::cout << "size: " << lexicalenvstack->Size() << std::endl;

            if(lexicalenvstack->GetLexicalEnv(tier)[index] == nullptr){
                HandleError("#TESTIN: lexicalenv is null");
            }

            auto identifier_name = lexicalenvstack->Get(tier, index);
            auto identifier_name_expression =  enc->GetIdentifierByName(identifier_name);
            
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                                            identifier_name_expression,
                                                                            source_expression,
                                                                            BinIntrinsicIdToToken(compiler::RuntimeInterface::IntrinsicId::ISIN_IMM8_V8)
                                                                        );

            enc->SetExpressionByRegister(inst, inst->GetDstReg(), binexpression);
            break;
            
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDPATCHVAR_PREF_IMM16:
       {
            auto patchvar_offset = static_cast<uint32_t>(inst->GetImms()[0]);
            if (enc->patchvarspace_->find(patchvar_offset) != enc->patchvarspace_->end()) {
                std::string *identifier_name =  (*enc->patchvarspace_)[patchvar_offset];
                enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->GetIdentifierByName(identifier_name));

            } else {
                HandleError("load patchvar Index does not exist in the map");
            }
            
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_STPATCHVAR_PREF_IMM16:
       {
            auto raw_expression  = *enc->GetExpressionByAcc(inst);
            auto patchvar_offset = static_cast<uint32_t>(inst->GetImms()[0]);
            std::string closure_name =  "closure_" + std::to_string(enc->methodoffset_) + "_" + std::to_string(enc->closure_count);
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                enc->GetIdentifierByName(closure_name),
                                                                                raw_expression,
                                                                                es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            ); 

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            (*enc->patchvarspace_)[patchvar_offset] = new std::string(closure_name);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLE_PREF_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLEWITHNAME_PREF_ID16:
       case compiler::RuntimeInterface::IntrinsicId::THROW_NOTEXISTS_PREF_NONE:
       case compiler::RuntimeInterface::IntrinsicId::THROW_PATTERNNONCOERCIBLE_PREF_NONE:
       case compiler::RuntimeInterface::IntrinsicId::THROW_DELETESUPERPROPERTY_PREF_NONE:
       case compiler::RuntimeInterface::IntrinsicId::THROW_CONSTASSIGNMENT_PREF_V8:
       case compiler::RuntimeInterface::IntrinsicId::THROW_IFNOTOBJECT_PREF_V8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NOTIFYCONCURRENTRESULT_PREF_NONE:
       case compiler::RuntimeInterface::IntrinsicId::GETMODULENAMESPACE_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_GETMODULENAMESPACE_PREF_IMM16:
       {
           // nothing todo
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8:
       case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8:
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINESENDABLECLASS_PREF_IMM16_ID16_ID16_IMM16_V8:
       {
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            auto constructor_offset = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->current_constructor_offset = constructor_offset;
            auto constructor_offset_name = enc->ir_interface_->GetMethodIdByOffset(constructor_offset);

            CopyLexicalenvStack(constructor_offset, inst, enc->method2lexicalenvstack_, enc->bb2lexicalenvstack_, enc->globallexical_waitlist_);
            CopyLexicalenvStack(constructor_offset, inst, enc->method2sendablelexicalenvstack_, enc->bb2sendablelexicalenvstack_, enc->globalsendablelexical_waitlist_);

            if (enc->class2memberfuns_->find(constructor_offset) != enc->class2memberfuns_->end()) {
                auto& member_funcs = (*enc->class2memberfuns_)[constructor_offset];
                for (const auto& member_func_offset : member_funcs) {
                    CopyLexicalenvStack(member_func_offset, inst, enc->method2lexicalenvstack_, enc->bb2lexicalenvstack_, enc->globallexical_waitlist_);
                    CopyLexicalenvStack(member_func_offset, inst, enc->method2sendablelexicalenvstack_, enc->bb2sendablelexicalenvstack_, enc->globalsendablelexical_waitlist_);
                }
            }
            
            auto father = *enc->GetExpressionByRegIndex(inst, 0);
            
            if(father != enc->constant_hole){
                auto fathername = enc->GetNameFromExpression(father);
                if(fathername){
                    std::cout << "father is " << *fathername << std::endl;
                }
            }else{
                father = nullptr;
                std::cout << "father is object" << std::endl;
            }

            (*enc->class2father_)[constructor_offset] = father;
       
            auto newname = RemoveArgumentsOfFunc(constructor_offset_name);
            auto newexpression = enc->GetIdentifierByName(newname);
            
            enc->not_add_assgin_for_stlexvar.insert(newexpression);
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), newexpression);
            
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CREATEPRIVATEPROPERTY_PREF_IMM16_ID16:
       {
            auto startpos = SearchStartposForCreatePrivateproperty(inst, enc->bb2lexicalenvstack_, enc->method2lexicalmap_, enc->methodoffset_);
            auto literalarray_offset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto member_functions = GetLiteralArrayByOffset(enc->program_, literalarray_offset);
            if(member_functions){
                for(const auto &member_function: *member_functions){
                    auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
                    auto &lexicalenv = lexicalenvstack->Top();
                    
                    // std::cout << "[+] size: " << lexicalenvstack->Size() << std::endl;
                    // std::cout << "[+] env size: " << lexicalenvstack->GetLexicalEnv(0).Size() << std::endl;
                    // std::cout << "[+] capacity_: " << lexicalenv.capacity_ << std::endl;

                    uint32_t member_offset = 0;
                    if (enc->methodname2offset_->find(member_function) != enc->methodname2offset_->end()) {
                        member_offset = (*enc->methodname2offset_)[member_function];
                    }else{
                        std::cout << "##name: " << member_function << std::endl;
                        HandleError("#DEFINEMETHOD: find constructor_offset error");
                    }


                    auto newname = enc->RemovePrefixOfFunc(member_function);
                    auto memfun_str = new std::string(newname);

                    // std::cout << "@@@: " << member_function << std::endl;
                    // std::cout << *memfun_str << std::endl;
            
                    lexicalenv.Set(startpos, memfun_str);
                    DealWithGlobalLexicalWaitlist(0, startpos++, *memfun_str, enc->globallexical_waitlist_);

                    CopyLexicalenvStack(member_offset, inst, enc->method2lexicalenvstack_, enc->bb2lexicalenvstack_, enc->globallexical_waitlist_);

                    // std::cout << "-----------------------------------------------------------------------------" << std::endl;
                    // std::cout << "[-] size: " << lexicalenvstack->Size() << std::endl;
                    // std::cout << "[-] env size: " << lexicalenvstack->GetLexicalEnv(0).Size() << std::endl;
                    // std::cout << "[-] capacity_: " << lexicalenv.capacity_ << std::endl;
                }
            }

            PrintInnerMethod2LexicalMap(enc->method2lexicalmap_, enc->methodoffset_);
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEPRIVATEPROPERTY_PREF_IMM8_IMM16_IMM16_V8:
       {
            auto tier = static_cast<uint32_t>(inst->GetImms()[1]);
            auto index = static_cast<uint32_t>(inst->GetImms()[2]);
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];

            std::string* privatevar = new std::string("p" + std::to_string(enc->privatevar_count++));
            auto privateid = enc->GetIdentifierByName(privatevar);

            auto obj_reg_identifier = *enc->GetExpressionByRegIndex(inst, 0);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                obj_reg_identifier,
                                                                                privateid,
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS,
                                                                                false,
                                                                                false
                                                                            );

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  objattrexpression,
                                                                                  *enc->GetExpressionByAcc(inst),
                                                                                  es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);

            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            lexicalenvstack->Set(tier, index, privatevar);
            DealWithGlobalLexicalWaitlist(tier, index, *privatevar, enc->globallexical_waitlist_);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDPRIVATEPROPERTY_IMM8_IMM16_IMM16:
       {
            auto tier = static_cast<uint32_t>(inst->GetImms()[1]);
            auto index = static_cast<uint32_t>(inst->GetImms()[2]);
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            if(lexicalenvstack->GetLexicalEnv(tier)[index] == nullptr){
                HandleError("#LDLEXVAR: lexicalenv is null");
            }

            auto attr_name = lexicalenvstack->Get(tier, index);
            auto attr_expression = enc->GetIdentifierByName(attr_name);

            auto obj_expression = *enc->GetExpressionByAcc(inst);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                obj_expression,
                                                                                attr_expression,
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                                false, 
                                                                                false);
            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STPRIVATEPROPERTY_IMM8_IMM16_IMM16_V8:
       {
            auto tier = static_cast<uint32_t>(inst->GetImms()[1]);
            auto index = static_cast<uint32_t>(inst->GetImms()[2]);
            auto lexicalenvstack = enc->bb2lexicalenvstack_[inst->GetBasicBlock()];
            if(lexicalenvstack->GetLexicalEnv(tier)[index] == nullptr){
                HandleError("#LDLEXVAR: lexicalenv is null");
            }
            auto attr_name = lexicalenvstack->Get(tier, index);
            auto attr_expression = enc->GetIdentifierByName(attr_name);

            auto obj_expression = *enc->GetExpressionByRegIndex(inst, 0);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                obj_expression,
                                                                                attr_expression,
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                                false, 
                                                                                false
                                                                            );   

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  objattrexpression,
                                                                                  *enc->GetExpressionByAcc(inst),
                                                                                  es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc,
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }
        
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CALLINIT_PREF_IMM8_V8:
       {
            auto funobj = *enc->GetExpressionByAcc(inst);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funobj,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);

            enc->thisptr = *enc->GetExpressionByRegIndex(inst, 0);
            break;
        }
        
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_SUPERCALLFORWARDALLARGS_PREF_V8:
       {
            enc->HandleNewCreatedExpression(inst, *enc->GetExpressionByRegIndex(inst, 0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM16_ID16:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto attr_expression = enc->GetIdentifierByName(str);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("super"),
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);
            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
        }
        
       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM16_V8:
       {
            auto attr_expression = *enc->GetExpressionByAcc(inst);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("super"),
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM16_ID16_V8:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("super"),
                                                        enc->GetIdentifierByName(str),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);  
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }
       

       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM8_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM16_V8_V8:
       {
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("super"),
                                                        *enc->GetExpressionByRegIndex(inst, 1),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);  
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

        
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM8_ID16: // hard to trigger
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM16_ID16:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto attr_expression = enc->GetIdentifierByName(str);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("this"),
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);

            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;
        } 
       
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM8: // hard to trigger
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM16:
       {
            auto attr_expression = *enc->GetExpressionByAcc(inst);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        enc->GetIdentifierByName("this"),
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            enc->HandleNewCreatedExpression(inst, objattrexpression);
            break;

        }
       
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM16_ID16:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                enc->GetIdentifierByName("this"),
                                                                                enc->GetIdentifierByName(str),
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                                false, 
                                                                                false);

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    objattrexpression,
                                                                                    *enc->GetExpressionByAcc(inst),
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM16_V8:
       {
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                enc->GetIdentifierByName("this"),
                                                                                *enc->GetExpressionByRegIndex(inst, 1),
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                                true, 
                                                                                false); 

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    objattrexpression,
                                                                                    *enc->GetExpressionByAcc(inst),
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                ); 

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }
       
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NEWSENDABLEENV_PREF_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDENEWSENDABLEENV_PREF_IMM16:
        {
            auto lexenv_size = static_cast<uint32_t>(inst->GetImms()[0]);

            auto lexicalenvstack = enc->bb2sendablelexicalenvstack_[inst->GetBasicBlock()];
            
            if(lexicalenvstack){
                std::cout << "not null" << std::endl;
            }else{
                std::cout << "null" << std::endl;
            }
        
            lexicalenvstack->Push(lexenv_size);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLECLASS_PREF_IMM16:
       {
            std::cout << "@@@ ldsendableclass >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
            std::cout << enc->methodoffset_ << std::endl;
            auto tier = static_cast<uint32_t>(inst->GetImms()[0]);
            auto index = 0;

            std::cout << "tier: " << std::to_string(tier) << ", index: " << std::to_string(index) << std::endl;

            LexicalEnvStack* lexicalenvstack;
            lexicalenvstack = enc->bb2sendablelexicalenvstack_[inst->GetBasicBlock()];
            std::cout << "size: " << lexicalenvstack->Size() << std::endl;

            if(lexicalenvstack->GetLexicalEnv(tier)[index] == nullptr){
                HandleError("#LDLEXVAR: lexicalenv is null");
            }

            auto identifier_name = lexicalenvstack->Get(tier, index);
            enc->SetExpressionByRegister(inst, inst->GetDstReg(), enc->GetIdentifierByName(enc->RemovePrefixOfFunc(*identifier_name)));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DEFINEPROPERTYBYNAME_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::DEFINEFIELDBYNAME_IMM8_ID16_V8:
       {
            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);

            ArenaVector<es2panda::ir::Expression *> new_properties(enc->parser_program_->Allocator()->Adapter());
            auto raw_obj = *enc->GetExpressionByRegIndex(inst, 0);
            if(raw_obj->IsObjectExpression()){
                auto objexpression = raw_obj->AsObjectExpression();
                for (auto *it : objexpression->Properties()) {
                    switch (it->Type()) {
                        case es2panda::ir::AstNodeType::PROPERTY: {
                            auto props = it->AsProperty();
                            auto key = props->Key();
                            auto keyname = enc->GetNameFromExpression(key);
                            if(keyname && *keyname == str){
                                new_properties.push_back(AllocNode<es2panda::ir::Property>(enc, key, *enc->GetExpressionByAcc(inst)));
                            }else{
                                new_properties.push_back(it);
                            }
                            
                            break;
                        }
                        default: {
                            new_properties.push_back(it);
                            break;
                        }
                    }
                }
                auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                                    es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                                    std::move(new_properties),
                                                                                    false);
                enc->SetExpressionByRegister(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), objectexpression);
            }else if(raw_obj->IsIdentifier()){
                auto attr_expression = enc->GetIdentifierByName(str);
                auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                            raw_obj,
                                                            attr_expression, 
                                                            es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                            true, 
                                                            false);
                auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    objattrexpression,
                                                                                    *enc->GetExpressionByAcc(inst),
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );

                auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
                enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            }else{
                std::cout << "###: " << std::to_string(static_cast<int>(raw_obj->Type())) << std::endl;
                HandleError("#STARRAYSPREAD: cann't deal expression except ObjectExpression");

            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DYNAMICIMPORT:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            panda::es2panda::ir::Identifier* funname = enc->GetIdentifierByName("import");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(source_expression);

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            funname,
                                                                            std::move(arguments),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYINDEX_PREF_IMM8_IMM32_V8:
       {
            auto imm = static_cast<uint32_t>(inst->GetImms()[1]);
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByReg(inst->GetSrcReg(0));
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                               obj_reg_identifier,
                                                                               enc->GetLiteralByNum(imm),  
                                                                               es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                               true, 
                                                                               false);

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        );

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);

            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYVALUE_PREF_IMM8_V8_V8:
       {
            panda::es2panda::ir::Expression* attr_expression = *enc->GetExpressionByRegIndex(inst, 0);
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByRegIndex(inst, 1);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        );

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);

            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLSPREAD_IMM8_V8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("super");
            auto source_expression =  *enc->GetExpressionByRegIndex(inst, 0);
            auto arrayexpression = source_expression->AsArrayExpression();
            auto constElements = arrayexpression->Elements();
            auto &nonConstElements = const_cast<ArenaVector<es2panda::ir::Expression *> &>(constElements);

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(nonConstElements),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM8_ID16_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM16_ID16_IMM8:
       {
           // 0（none），1（g），2（i），4（m），8（s），16（u），32（y）；3 -> gi。

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::Expression *callee = *enc->GetExpressionByRegIndex(inst, 0);

            auto stroffset = static_cast<uint32_t>(inst->GetImms()[1]);
            auto str = enc->ir_interface_->GetStringIdByOffset(stroffset);
            arguments.push_back(enc->GetIdentifierByName(str));

            auto imm = static_cast<uint32_t>(inst->GetImms()[2]);
            arguments.push_back(enc->GetLiteralByNum(imm));

            es2panda::ir::Expression *newExprNode = AllocNode<es2panda::ir::NewExpression>(enc, callee, nullptr, std::move(arguments));

            enc->HandleNewCreatedExpression(inst, newExprNode);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHEXCLUDEDKEYS_IMM8_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_CREATEOBJECTWITHEXCLUDEDKEYS_PREF_IMM16_V8_V8:
       {
            uint32_t argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            es2panda::ir::Expression* rawobj;
            rawobj = *enc->GetExpressionByRegIndex(inst, 0);

            es2panda::ir::ObjectExpression* obj = nullptr;
            es2panda::ir::Expression* tmpobj = nullptr;

            if(rawobj->IsIdentifier()){
                tmpobj = enc->globalname2expression_[rawobj->AsIdentifier()];
                if(tmpobj->IsObjectExpression()){
                    obj = tmpobj->AsObjectExpression();
                }else{
                    HandleError("#CREATEOBJECTWITHEXCLUDEDKEYS @1");
                }
            }else if(rawobj->IsObjectExpression()){
                obj = rawobj->AsObjectExpression();
            }else{
                HandleError("#CREATEOBJECTWITHEXCLUDEDKEYS @2");
            }
            
            auto objprops = obj->Properties();
            ArenaVector<es2panda::ir::Expression *> properties(enc->parser_program_->Allocator()->Adapter());

            std::set<std::string> props;
            for (uint32_t i = 1; i <= argsum+1; ++i) {
                auto rawattr = *enc->GetExpressionByRegIndex(inst, i);
                auto idname = enc->GetNameFromExpression(rawattr);

                if(idname){
                    props.insert(*idname);
                }else{
                    std::cout << "error type: " << std::to_string( static_cast<int>(rawattr->Type()) ) << std::endl;
                    HandleError("#CREATEOBJECTWITHEXCLUDEDKEYS @3");
                }
            }

            for(uint32_t i = 0; i < objprops.size(); i++){
                auto rawattrkey = objprops[i];
                if(rawattrkey->IsProperty()){
                    auto rawprop = rawattrkey->AsProperty();
                    auto rawkey = rawprop->Key();

                    auto idname = enc->GetNameFromExpression(rawkey);
                    if(!idname){
                        std::cout << "error type: " << std::to_string( static_cast<int>(rawkey->Type()) ) << std::endl;
                        HandleError("#CREATEOBJECTWITHEXCLUDEDKEYS @4");
                    }

                    if(props.find(*idname) == props.end()){
                        properties.push_back(  rawattrkey );
                    }
                }else{
                    HandleError("#CREATEOBJECTWITHEXCLUDEDKEYS @5");
                }                
            }

            auto objectexpression = AllocNode<es2panda::ir::ObjectExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::OBJECT_EXPRESSION,
                                                                            std::move(properties),
                                                                            false
                                                                            );
            enc->HandleNewCreatedExpression(inst, objectexpression);

            // just for testing
            // auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
            //                                                                     enc->GetIdentifierByName("tt"),
            //                                                                     objectexpression,
            //                                                                     es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
            //                                                                 );
            // auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            // enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM16:
       {
            panda::es2panda::ir::Expression* acc = *enc->GetExpressionByAcc(inst);
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("GetTemplateObject");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(acc);

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM16_V8:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByAcc(inst);
            auto value = *enc->GetExpressionByRegIndex(inst, 0);

            auto attr_expression = enc->GetIdentifierByName("__proto__");
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                                                obj_expression,
                                                                                attr_expression,
                                                                                es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                                                false, 
                                                                                false
                                                                            );   
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  objattrexpression,
                                                                                  value,
                                                                                  es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc,
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_TOPROPERTYKEY_PREF_NONE:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByAcc(inst);
            enc->HandleNewCreatedExpression(inst, obj_expression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DEFINEGETTERSETTERBYVALUE_V8_V8_V8_V8:
       {
            es2panda::ir::Expression *rawmember = *enc->GetExpressionByRegIndex(inst, 0);
            uint32_t constructor_offset;
            if(rawmember->IsMemberExpression()){
                auto rawobj = rawmember->AsMemberExpression()->Object();
                auto objname = enc->GetNameFromExpression(rawobj);
                if(objname){
                    if(enc->methodname2offset_->find(*objname) != enc->methodname2offset_->end()){
                        constructor_offset = (*enc->methodname2offset_)[*objname];
                    }else{
                        HandleError("#DEFINEGETTERSETTERBYVALUE: not support this case1"); 
                    }
                }else{
                    std::cout << "###: " << std::to_string(static_cast<int>(rawobj->Type())) << std::endl;
                    HandleError("#DEFINEGETTERSETTERBYVALUE: not support this case2"); 
                }
            }else{
                std::cout << "###: " << std::to_string(static_cast<int>(rawmember->Type())) << std::endl;
                HandleError("#DEFINEGETTERSETTERBYVALUE: not support this case3");       
            }

            es2panda::ir::Expression *rawcallee1 = *enc->GetExpressionByRegIndex(inst, 2);
            auto rawcalleename1 = enc->GetNameFromExpression(rawcallee1);
            if(rawcalleename1){
                auto tmp = RemoveArgumentsOfFunc(*rawcalleename1);
                if(enc->methodname2offset_->find(tmp) != enc->methodname2offset_->end()){
                    auto methodoffset = (*enc->methodname2offset_)[tmp];
                    (*enc->class2memberfuns_)[constructor_offset].insert(methodoffset);
                    enc->memberfuncs_->insert(methodoffset);
                }
            }else{
                std::cout << "###: " << std::to_string(static_cast<int>(rawcallee1->Type())) << std::endl;
                HandleError("#DEFINEGETTERSETTERBYVALUE: not support this case5");
            }

            es2panda::ir::Expression *rawcallee2 = *enc->GetExpressionByRegIndex(inst, 3);
            auto rawcalleename2 = enc->GetNameFromExpression(rawcallee2);
            if(rawcalleename2){
                auto tmp = RemoveArgumentsOfFunc(*rawcalleename2);
                if(enc->methodname2offset_->find(tmp) != enc->methodname2offset_->end()){
                    auto methodoffset = (*enc->methodname2offset_)[tmp];
                    (*enc->class2memberfuns_)[constructor_offset].insert(methodoffset);
                    enc->memberfuncs_->insert(methodoffset);
                }   
            }else{
                std::cout << "###: " << std::to_string(static_cast<int>(rawcallee2->Type())) << std::endl;
                HandleError("#DEFINEGETTERSETTERBYVALUE: not support this case6");
            }
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONENTER:
        {
            enc->MarkAsync();

            enc->HandleNewCreatedExpression(inst, enc->constant_asyncfuncmark);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONAWAITUNCAUGHT_V8:
        {
            // panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            // auto awaitexpression = AllocNode<es2panda::ir::AwaitExpression>(enc,  source_expression);
            // enc->HandleNewCreatedExpression(inst, awaitexpression);

            enc->MarkAsync();

            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_await") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;
            
            panda::es2panda::ir::Expression* source_expression = *enc->GetExpressionByAcc(inst);
            auto awaitexpression = AllocNode<es2panda::ir::AwaitExpression>(enc,  source_expression);
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  awaitexpression,
                                                                                  es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::SUSPENDGENERATOR_V8:
        {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("suspendgenerator");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);

            auto obj_expression = *enc->GetExpressionByAcc(inst);
            enc->HandleNewCreatedExpression(inst, obj_expression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::RESUMEGENERATOR:{
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_resumegenerator") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("GeneratorResume");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );


            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);
            
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONRESOLVE_V8:
        {
            enc->MarkAsync();
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByAcc(inst);
            enc->HandleNewCreatedExpression(inst, obj_expression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONREJECT_V8:{
            enc->MarkAsync();
            enc->HandleNewCreatedExpression(inst, enc->constant_rejectmode); 
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::GETRESUMEMODE:
        {
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_getresumemode") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("GetResumeMode");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );


            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEGENERATOROBJ_V8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("CreateGeneratorObj");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_generatorobj") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEITERRESULTOBJ_V8_V8:
       {
            es2panda::ir::Expression *obj_expression = *enc->GetExpressionByRegIndex(inst, 0);
            auto yieldrexpression = AllocNode<es2panda::ir::YieldExpression>(enc, obj_expression, false);

            auto yieldstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, yieldrexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, yieldstatement);
            ////////////////////////////////////////////////////////////////////////////////////////////////
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_createresultobj") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("CreateIterResultObject");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );


            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);
            
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORRESOLVE_V8_V8_V8:
       {
            enc->MarkAsync();
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("AsyncGeneratorResolve");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 2));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_asyncgeneratorresolve") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                  es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SETGENERATORSTATE_IMM8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("setgeneratorstate");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            auto statevalue = static_cast<uint32_t>(inst->GetImms()[0]);
            arguments.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, statevalue));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);

            auto obj_expression = *enc->GetExpressionByAcc(inst);
            enc->HandleNewCreatedExpression(inst, obj_expression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEASYNCGENERATOROBJ_V8:
       {
            enc->MarkAsync();
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("CreateAsyncGeneratorObj");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));
            // arguments.push_back(*enc->GetExpressionByRegIndex(inst, 1));
            // arguments.push_back(*enc->GetExpressionByRegIndex(inst, 2));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_asyncgeneratorobj") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORREJECT_V8:
       {
            enc->MarkAsync();
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("AsyncGeneratorReject");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));
            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_asyncgeneratorreject") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::GETNEXTPROPNAME_V8:
       {
            auto memberfuncname = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->GetExpressionByRegIndex(inst, 0),
                                                        enc->GetIdentifierByName("next"), 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                memberfuncname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->HandleNewCreatedExpression(inst, callexpression);

            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_asyncgeneratorobj") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);

            

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::GETPROPITERATOR:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("GetPropIterator");
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            es2panda::ir::CallExpression* callexpression = AllocNode<es2panda::ir::CallExpression>(enc,
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_iterator") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;
            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);


            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::GETASYNCITERATOR_IMM8:
       {
            panda::es2panda::ir::Expression* funname = nullptr;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::GETASYNCITERATOR_IMM8){
                funname = enc->GetIdentifierByName("GetAsyncIterator");
            }else{
                funname = enc->GetIdentifierByName("GetIterator");
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByAcc(inst));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );

            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->GetIdentifierByName(std::string("ret_iterator") + "_" + std::to_string(enc->closure_count));
            enc->closure_count++;

            auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                  obj_reg_identifier,
                                                                                  callexpression,
                                                                                   es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                            );
                                                                            
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);

            enc->HandleNewCreatedExpression(inst, obj_reg_identifier);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM16_V8:
       {
            panda::es2panda::ir::Expression* funname = enc->GetIdentifierByName("IteratorClose");

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->GetExpressionByRegIndex(inst, 0));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );
            enc->HandleNewCreatedExpression(inst, callexpression);
            break;
        }
              
       
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM8_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM16_IMM16:
        {
            panda::es2panda::ir::Expression* obj_expression = *enc->GetExpressionByAcc(inst);
            
            uint32_t imm;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_LDOBJBYINDEX_PREF_IMM32){
                imm = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                imm = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            panda::es2panda::ir::Expression* attr_expression = enc->GetLiteralByNum(imm);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            enc->SetExpressionByRegister(inst, inst->GetDstReg(), objattrexpression);
            break;

        }

       case compiler::RuntimeInterface::IntrinsicId::STOBJBYINDEX_IMM8_V8_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYINDEX_IMM16_V8_IMM16:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_STOBJBYINDEX_PREF_V8_IMM32:
       {
            uint32_t imm;
            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_STOBJBYINDEX_PREF_V8_IMM32){
                imm = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                imm = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->GetExpressionByRegIndex(inst, 0),
                                                        enc->GetLiteralByNum(imm), 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

             panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->GetExpressionByAcc(inst),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->AddInstAst2BlockStatemntByInst(inst, assignstatement);
            break;        
        
        }
       
        default:
            std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
            enc->success_ = false;
            LOG(ERROR,COMPILER) << "Unsupported ecma opcode";
    }
}
