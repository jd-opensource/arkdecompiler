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
            enc->add_insAst_to_blockstatemnt_by_inst(inst, returnstatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDINFINITY:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_infinity);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDBIGINT_ID16:
       {
            panda::es2panda::ir::Identifier* funname = enc->get_identifier_byname(new std::string("BigInt"));
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            std::string* source_str_ptr = new std::string(bc_id0);
            es2panda::util::StringView literal_strview(*source_str_ptr);

            arguments.push_back( AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            funname,
                                                                            std::move(arguments),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression); 
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDHOLE:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_hole);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDUNDEFINED:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_undefined);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDNULL:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_null);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTRUE:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_true);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDFALSE:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_false);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDNAN:
       {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->constant_nan);
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
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);  
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            *enc->get_expression_by_id(inst, 0),
                                                            source_expression,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );
            enc->set_expression_by_register(inst, inst->GetDstReg(), binexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::INC_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::DEC_IMM8:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_one,
                                                            IncDecIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), binexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::ISTRUE:{
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_true,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), binexpression);
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ISFALSE:{
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            auto binexpression = AllocNode<es2panda::ir::BinaryExpression>(enc, 
                                                            source_expression,
                                                            enc->constant_false,
                                                            BinIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), binexpression);
            break;

        }

       case compiler::RuntimeInterface::IntrinsicId::NOT_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::NEG_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TYPEOF_IMM16: {
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);        

            auto unaryexpression = AllocNode<es2panda::ir::UnaryExpression>(enc, 
                                                            source_expression,
                                                            UnaryPrefixIntrinsicIdToToken(inst->GetIntrinsicId())
            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), unaryexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::TONUMBER_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::TONUMERIC_IMM8:
       {
            panda::es2panda::ir::Expression* source_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            panda::es2panda::ir::Identifier* funname = enc->get_identifier_byname(new std::string("Number"));
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(source_expression);

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            funname,
                                                                            std::move(arguments),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STGLOBALVAR_IMM16_ID16:
       case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::TRYSTGLOBALBYNAME_IMM16_ID16:
       {
            panda::es2panda::ir::Expression* src_reg_identifier = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);  

            if(src_reg_identifier != enc->DEFINEFUNC){
                auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
                auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
                std::string* global_name = new std::string(bc_id0);

                auto globalvar = enc->get_identifier_byname(global_name);

                auto assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                                    globalvar,
                                                                                    src_reg_identifier,
                                                                                    es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                                );
                auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, assignexpression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);
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
            enc->set_expression_by_register(inst, inst->GetDstReg(), objectexpression);
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


            enc->set_expression_by_register(inst, inst->GetDstReg(), arrayexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::CREATEARRAYWITHBUFFER_IMM16_ID16:
        {
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());

            ASSERT(inst->HasImms() && inst->GetImms().size() > 0);
            
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1);
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);

            auto literalarray = enc->findLiteralArrayByOffset(ir_id0);
            if(!literalarray){
                handleError("get literalarray error");
            }

            /*
                std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, float, double, std::string> value_;

            */
            for (const auto& literal : literalarray->literals_) {
                if(literal.IsBoolValue()){
                    elements.push_back(AllocNode<es2panda::ir::BooleanLiteral>(enc, std::get<bool>(literal.value_)));
                }else if(literal.IsByteValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint8_t>(literal.value_)));
                }else if(literal.IsShortValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint16_t>(literal.value_)));
                }else if(literal.IsIntegerValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint32_t>(literal.value_)));
                }else if(literal.IsLongValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<uint64_t>(literal.value_)));
                }else if(literal.IsFloatValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<float>(literal.value_)));
                }else if(literal.IsDoubleValue()){
                    elements.push_back(AllocNode<es2panda::ir::NumberLiteral>(enc, std::get<double>(literal.value_)));
                }else if(literal.IsStringValue()){
                    es2panda::util::StringView literal_strview(std::get<std::string>(literal.value_));

                    elements.push_back(AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview));
                }else{
                    handleError("unsupport literal type error");
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

            enc->set_expression_by_register(inst, inst->GetDstReg(), arrayexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHBUFFER_IMM16_ID16:
        {
            ArenaVector<es2panda::ir::Expression *> properties(enc->parser_program_->Allocator()->Adapter());
            
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]); 
            auto literalarray = enc->findLiteralArrayByOffset(ir_id0);

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
                    es2panda::util::StringView literal_strview(std::get<std::string>(literal.value_));
                    tmp = AllocNode<es2panda::ir::StringLiteral>(enc, literal_strview);
                }else{
                    handleError("unsupport literal type error");
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

            ArenaVector<es2panda::ir::VariableDeclarator *> declarators(enc->parser_program_->Allocator()->Adapter());


            enc->set_expression_by_register(inst, inst->GetDstReg(), objectexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::LDGLOBALVAR_IMM16_ID16:
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM8_ID16:
        case compiler::RuntimeInterface::IntrinsicId::TRYLDGLOBALBYNAME_IMM16_ID16:
       {
            ASSERT(inst->HasImms() && inst->GetImms().size() > 0); 
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); 

            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            es2panda::ir::Expression* sourceexpression = enc->get_identifier_byname(new std::string(bc_id0));
            enc->set_expression_by_register(inst, inst->GetDstReg(), sourceexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM8_ID16:
       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYNAME_IMM16_ID16:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            std::string* name_ptr= new std::string(bc_id0);

            panda::es2panda::ir::Expression* attr_expression = enc->get_identifier_byname(name_ptr);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);


            enc->set_expression_by_register(inst, inst->GetDstReg(), objattrexpression);
            break;
       }

       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM16_V8:
       case compiler::RuntimeInterface::IntrinsicId::LDOBJBYVALUE_IMM8_V8:
       {
            panda::es2panda::ir::Expression* attr_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            panda::es2panda::ir::Expression* obj_expression = *enc->get_expression_by_id(inst, 0);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            enc->set_expression_by_register(inst, inst->GetDstReg(), objattrexpression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::STTOGLOBALRECORD_IMM16_ID16:
        case compiler::RuntimeInterface::IntrinsicId::STCONSTTOGLOBALRECORD_IMM16_ID16:
        {
            panda::es2panda::ir::Expression* src_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            std::string* global_name = new std::string(bc_id0);
            
            auto dst_identifier = enc->get_identifier_byname(global_name);

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
                enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration);
            }else{
                auto variadeclaration = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::LET,
                                                                                    std::move(declarators),
                                                                                    true
                                                                                );
                enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration);
            }

            

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLARG0_IMM8:
        {
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::CALLARG1_IMM8_V8:
        {
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);


            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            auto v0 = *enc->get_expression_by_id(inst, 0);
            
            arguments.push_back(v0);
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLARGS2_IMM8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(*enc->get_expression_by_id(inst, 0));
            arguments.push_back(*enc->get_expression_by_id(inst, 1));


            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;

        }
       case compiler::RuntimeInterface::IntrinsicId::CALLARGS3_IMM8_V8_V8_V8:
       {
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);


            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->get_expression_by_id(inst, 0));
            arguments.push_back(*enc->get_expression_by_id(inst, 1));
            arguments.push_back(*enc->get_expression_by_id(inst, 2));

            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;
        }
       
       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS0_IMM8_V8:
       {
            
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            enc->thisptr = *enc->get_expression_by_id(inst, 0);

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            
            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS1_IMM8_V8_V8:
       {
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->get_expression_by_id(inst, 0);
            arguments.push_back(*enc->get_expression_by_id(inst, 1));

            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS2_IMM8_V8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->get_expression_by_id(inst, 0);
            arguments.push_back(*enc->get_expression_by_id(inst, 1));
            arguments.push_back(*enc->get_expression_by_id(inst, 2));

            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLTHIS3_IMM8_V8_V8_V8_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            enc->thisptr = *enc->get_expression_by_id(inst, 0);
            arguments.push_back(*enc->get_expression_by_id(inst, 1));
            arguments.push_back(*enc->get_expression_by_id(inst, 2));
            arguments.push_back(*enc->get_expression_by_id(inst, 3));

            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            if(inst->HasUsers()){
                enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            }else{
                auto callarg0statement = AllocNode<es2panda::ir::ExpressionStatement>(enc, callarg0expression);
                enc->add_insAst_to_blockstatemnt_by_inst(inst, callarg0statement);
            }

            break;
        }
       
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM8_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYVALUE_IMM16_V8_V8:
       {
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->get_expression_by_id(inst, 0),
                                                        *enc->get_expression_by_id(inst, 1), 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);
            panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);
            break;

        }


       case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM16_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOBJBYNAME_IMM8_ID16_V8:
       {
            ASSERT(inst->HasImms() && inst->GetImms().size() > 0); 
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); 
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            std::string* global_name = new std::string(bc_id0);
            
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        *enc->get_expression_by_id(inst, 0),
                                                        enc->get_identifier_byname(global_name),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);


            panda::es2panda::ir::Expression* assignexpression = AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
                                                                        
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::NEWOBJRANGE_IMM16_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWOBJRANGE_PREF_IMM16_V8:
       {
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            es2panda::ir::Expression *callee = *enc->get_expression_by_id(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_NEWOBJRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            for (uint32_t i = 1; i < argsum; ++i) {
                arguments.push_back(*enc->get_expression_by_id(inst, i));
            }

            es2panda::ir::Expression *newExprNode = AllocNode<es2panda::ir::NewExpression>(enc, callee, nullptr, std::move(arguments));
            enc->set_expression_by_register(inst, inst->GetDstReg(), newExprNode);
            break;

        }

       case compiler::RuntimeInterface::IntrinsicId::COPYDATAPROPERTIES_V8:
       {
            auto src_obj = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto v0 = inst->GetSrcReg(0);

            auto target_obj = *enc->get_expression_by_id(inst, 0);

            auto target_objexpression = static_cast<panda::es2panda::ir::ObjectExpression*>(target_obj);
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

            enc->set_expression_by_register(inst, inst->GetDstReg(), objectexpression);
            enc->set_expression_by_register(inst->GetInput(0).GetInst(), v0, objectexpression);
            break;
        }



       case compiler::RuntimeInterface::IntrinsicId::APPLY_IMM8_V8_V8:
       {
            auto fun = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            auto raw_expression = *enc->get_expression_by_id(inst, 1);

            auto raw_array_expression = static_cast<const panda::es2panda::ir::ArrayExpression*>(raw_expression);

            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());

            for (auto *it :raw_array_expression->Elements()) {
                elements.push_back(it);
            }

            auto this_expression = *enc->get_expression_by_id(inst, 0);
            enc->thisptr = this_expression;
            
            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                            fun,
                                                                            std::move(elements),
                                                                            nullptr,
                                                                            false
                                                                            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STARRAYSPREAD_V8_V8:
       {
            auto element = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            es2panda::ir::Expression* spreadelement = AllocNode<es2panda::ir::SpreadElement>(enc, es2panda::ir::AstNodeType::SPREAD_ELEMENT, element);

            auto raw_obj = *enc->get_expression_by_id(inst, 0);
            auto raw_arrayexpression = static_cast<panda::es2panda::ir::ArrayExpression*>(raw_obj);

            ArenaVector<es2panda::ir::Expression *> elements(enc->parser_program_->Allocator()->Adapter());
            auto index_expression = *enc->get_expression_by_id(inst, 1);

            auto index_literal = static_cast<panda::es2panda::ir::NumberLiteral*>(index_expression);
            uint32_t index = index_literal->Number();

            for (auto *it : raw_arrayexpression->Elements()) {
                elements.push_back(it);
            }

            elements.insert(elements.begin() + index, spreadelement);
            auto arrayexpression = AllocNode<es2panda::ir::ArrayExpression>(enc, 
                                                                            es2panda::ir::AstNodeType::ARRAY_EXPRESSION,
                                                                            std::move(elements),
                                                                            false
                                                                            );
            enc->set_expression_by_register(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), arrayexpression);

            uint32_t size = elements.size();
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->get_literal_bynum(size));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::DEBUGGER:
       {
            auto debuggerstatement = AllocNode<es2panda::ir::DebuggerStatement>(enc );
            enc->add_insAst_to_blockstatemnt_by_inst(inst, debuggerstatement);
            break;
       }

       case compiler::RuntimeInterface::IntrinsicId::CALLRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLRANGE_PREF_IMM16_V8:
       {

            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_CALLRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 0; i < argsum; ++i) {
                arguments.push_back(*enc->get_expression_by_id(inst, i));
            }

            
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::WIDE_CALLTHISRANGE_PREF_IMM16_V8:
        case compiler::RuntimeInterface::IntrinsicId::CALLTHISRANGE_IMM8_IMM8_V8:
        {
            panda::es2panda::ir::Expression* funname = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            enc->thisptr =  *enc->get_expression_by_id(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_CALLTHISRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 1; i <= argsum; ++i) {
                arguments.push_back(*enc->get_expression_by_id(inst, i));
            }

            
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );
            enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DELOBJPROP_V8:
       {
            panda::es2panda::ir::Expression* obj_expression = *enc->get_expression_by_id(inst, 0);
            panda::es2panda::ir::Expression* attr_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);

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

            enc->set_expression_by_register(inst, inst->GetDstReg(), unaryexpression);
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISFALSE_PREF_IMM8:
       {
            panda::es2panda::ir::Expression* funname = enc->get_identifier_byname(new std::string("runtime.isfalse"));
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());

            arguments.push_back(*enc->get_expression_by_id(inst, inst->GetInputsCount() - 2));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_ISTRUE_PREF_IMM8:
       {
            panda::es2panda::ir::Expression* funname = enc->get_identifier_byname(new std::string("runtime.istrue"));
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            arguments.push_back(*enc->get_expression_by_id(inst, inst->GetInputsCount() - 2));

            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression);
            break;
        }


        case compiler::RuntimeInterface::IntrinsicId::RETURN:
        {
            auto returnexpression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2); 
            auto returnstatement = AllocNode<es2panda::ir::ReturnStatement>(enc,  returnexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, returnstatement);
            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM8_ID16_IMM8:
        case compiler::RuntimeInterface::IntrinsicId::DEFINEFUNC_IMM16_ID16_IMM8:
        {
            enc->set_expression_by_register(inst, inst->GetDstReg(), enc->DEFINEFUNC);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::THROW_PREF_NONE:
       {
            auto argument = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);

            auto throwStatement = AllocNode<es2panda::ir::ThrowStatement>(enc, argument);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, throwStatement);
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM16_V8_V8: // tobetested
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUEWITHNAMESET_IMM8_V8_V8:  // tobetested
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM16_V8_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYVALUE_IMM8_V8_V8:
       {
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->get_identifier_byreg(inst->GetSrcReg(0));
            auto expression1 = *enc->get_expression_by_id(inst, 0);

            enc->set_expression_by_register(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), obj_reg_identifier);
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators1(enc->parser_program_->Allocator()->Adapter());
            auto *declarator1 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                obj_reg_identifier, 
                                                                                expression1);
            declarators1.push_back(declarator1);
            auto variadeclaration1 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                    std::move(declarators1),
                                                                                    true
                                                                                );
            enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration1);
            
            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            panda::es2panda::ir::Identifier* index_reg_identifier = enc->get_identifier_byreg(inst->GetSrcReg(1));
            auto expression2 = *enc->get_expression_by_id(inst, 1);

            enc->set_expression_by_register(inst->GetInput(1).GetInst(), inst->GetSrcReg(0), index_reg_identifier);

            ArenaVector<es2panda::ir::VariableDeclarator *> declarators2(enc->parser_program_->Allocator()->Adapter());
            auto *declarator2 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                index_reg_identifier, 
                                                                                expression2);
            declarators2.push_back(declarator2);
            auto variadeclaration2 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                    std::move(declarators2),
                                                                                    true
                                                                                );
            enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration2);

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_reg_identifier,
                                                        index_reg_identifier, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);   
            panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);

            break;
        }

        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM8_V8_IMM16: // tobetested
        case compiler::RuntimeInterface::IntrinsicId::STOWNBYINDEX_IMM16_V8_IMM16: // tobetested
        case compiler::RuntimeInterface::IntrinsicId::WIDE_STOWNBYINDEX_PREF_V8_IMM32: // tobetested
       {
            uint32_t imm;
            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_STOWNBYINDEX_PREF_V8_IMM32){
                imm = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                imm = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->get_identifier_byreg(inst->GetSrcReg(0));
            auto expression1 = *enc->get_expression_by_id(inst, 0);

            enc->set_expression_by_register(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), obj_reg_identifier);
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators1(enc->parser_program_->Allocator()->Adapter());
            auto *declarator1 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                obj_reg_identifier, 
                                                                                expression1);
            declarators1.push_back(declarator1);
            auto variadeclaration1 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                    std::move(declarators1),
                                                                                    true
                                                                                );
            enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration1);
            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_reg_identifier,
                                                        enc->get_literal_bynum(imm),  
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);   
            panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);

            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAME_IMM16_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM8_ID16_V8:
       case compiler::RuntimeInterface::IntrinsicId::STOWNBYNAMEWITHNAMESET_IMM16_ID16_V8:
       {
            panda::es2panda::ir::Identifier* obj_reg_identifier = enc->get_identifier_byreg(inst->GetSrcReg(0));
            auto expression1 = *enc->get_expression_by_id(inst, 0);

            enc->set_expression_by_register(inst->GetInput(0).GetInst(), inst->GetSrcReg(0), obj_reg_identifier);
            ArenaVector<es2panda::ir::VariableDeclarator *> declarators1(enc->parser_program_->Allocator()->Adapter());
            auto *declarator1 = AllocNode<es2panda::ir::VariableDeclarator>(enc,
                                                                                obj_reg_identifier, 
                                                                                expression1);
            declarators1.push_back(declarator1);
            auto variadeclaration1 = AllocNode<es2panda::ir::VariableDeclaration>(enc, 
                                                                                    es2panda::ir::VariableDeclaration::VariableDeclarationKind::VAR,
                                                                                    std::move(declarators1),
                                                                                    true
                                                                                );
            enc->add_insAst_to_blockstatemnt_by_inst(inst, variadeclaration1);
            
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);

            std::string* global_name = new std::string(bc_id0);
            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_reg_identifier,
                                                        enc->get_identifier_byname(global_name),
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        false, 
                                                        false);   
            panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 
            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);

            break;
        }
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

       case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM8:
       case compiler::RuntimeInterface::IntrinsicId::GETITERATOR_IMM16:
       {
            std::cout << "111111111111111111111111111111" << std::endl;
            panda::es2panda::ir::Expression* funname = enc->get_identifier_byname(new std::string("GetIterator"));
            std::cout << "222222222222222222222222222222" << std::endl;
            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            std::cout << "333333333333333333333333333333" << std::endl;

            arguments.push_back(*enc->get_expression_by_id(inst, inst->GetInputsCount() - 2));
            std::cout << "555555555555555555555555555555" << std::endl;
            auto callexpression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                funname,
                                                                std::move(arguments),
                                                                nullptr,
                                                                false
                                                                );
            std::cout << "666666666666666666666666666666" << std::endl;
            std::cout << "777777777777777777777777777777" << std::endl;
            enc->set_expression_by_register(inst, inst->GetDstReg(), callexpression);
            std::cout << "888888888888888888888888888888" << std::endl;
 

            break;
        }

       
        case compiler::RuntimeInterface::IntrinsicId::NEWLEXENV_IMM8:{
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_NEWLEXENV(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM8_ID16_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_DEFINEMETHOD(imm0, bc_id0, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM8_ID16_ID16_IMM16_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id1 = static_cast<uint32_t>(inst->GetImms()[2]);
            auto bc_id1 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id1);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 3); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm3 = static_cast<uint32_t>(inst->GetImms()[3]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_DEFINECLASSWITHBUFFER(imm0, bc_id0, bc_id1, imm3, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::GETNEXTPROPNAME_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_GETNEXTPROPNAME(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_LDSUPERBYVALUE(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM4_IMM4:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_LDLEXVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM4_IMM4:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_STLEXVAR(imm0, imm1));
            break;
        }
       

       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM8_ID16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_LDSUPERBYNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


        

       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM8_ID16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_LDTHISBYNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM8_ID16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_STTHISBYNAME(imm0, bc_id0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_LDTHISBYVALUE(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_STTHISBYVALUE(imm0, v0));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::GETPROPITERATOR:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_GETPROPITERATOR());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM8_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CLOSEITERATOR(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::POPLEXENV:
       {
            enc->result_.emplace_back(pandasm::Create_POPLEXENV());
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::GETUNMAPPEDARGS:
       {
            enc->result_.emplace_back(pandasm::Create_GETUNMAPPEDARGS());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDGLOBAL:
       {
            enc->result_.emplace_back(pandasm::Create_LDGLOBAL());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDNEWTARGET:
       {
            enc->result_.emplace_back(pandasm::Create_LDNEWTARGET());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTHIS:
       {
            enc->result_.emplace_back(pandasm::Create_LDTHIS());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM8_ID16_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_CREATEREGEXPWITHLITERAL(imm0, bc_id0, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CREATEREGEXPWITHLITERAL_IMM16_ID16_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_CREATEREGEXPWITHLITERAL(imm0, bc_id0, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::DEFINECLASSWITHBUFFER_IMM16_ID16_ID16_IMM16_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id1 = static_cast<uint32_t>(inst->GetImms()[2]);
            auto bc_id1 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id1);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 3); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm3 = static_cast<uint32_t>(inst->GetImms()[3]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_DEFINECLASSWITHBUFFER(imm0, bc_id0, bc_id1, imm3, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_GETTEMPLATEOBJECT(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_SETOBJECTWITHPROTO(imm0, v0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::GETMODULENAMESPACE_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_GETMODULENAMESPACE(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STMODULEVAR_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_STMODULEVAR(imm0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDLOCALMODULEVAR_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_LDLOCALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDEXTERNALMODULEVAR_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_LDEXTERNALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
 
       



       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYVALUE_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_LDSUPERBYVALUE(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::LDLEXVAR_IMM8_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_LDLEXVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STLEXVAR_IMM8_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_STLEXVAR(imm0, imm1));
            break;
        }
       
       case compiler::RuntimeInterface::IntrinsicId::LDSUPERBYNAME_IMM16_ID16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_LDSUPERBYNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYNAME_IMM16_ID16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_LDTHISBYNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYNAME_IMM16_ID16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_STTHISBYNAME(imm0, bc_id0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDTHISBYVALUE_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_LDTHISBYVALUE(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STTHISBYVALUE_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_STTHISBYVALUE(imm0, v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORREJECT_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_ASYNCGENERATORREJECT(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::CLOSEITERATOR_IMM16_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CLOSEITERATOR(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDSYMBOL:
       {
            enc->result_.emplace_back(pandasm::Create_LDSYMBOL());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONENTER:
       {
            enc->result_.emplace_back(pandasm::Create_ASYNCFUNCTIONENTER());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDFUNCTION:
       {
            enc->result_.emplace_back(pandasm::Create_LDFUNCTION());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CREATEGENERATOROBJ_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CREATEGENERATOROBJ(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CREATEITERRESULTOBJ_V8_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_CREATEITERRESULTOBJ(v0, v1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CREATEOBJECTWITHEXCLUDEDKEYS_IMM8_V8_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_CREATEOBJECTWITHEXCLUDEDKEYS(imm0, v0, v1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM8_V8:
       {
            std::cout << "compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM8_V8 " << std::endl;
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_NEWOBJAPPLY(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM16_V8:
       {
            std::cout << "compiler::RuntimeInterface::IntrinsicId::NEWOBJAPPLY_IMM16_V8 " << std::endl;
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_NEWOBJAPPLY(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::NEWLEXENVWITHNAME_IMM8_ID16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_NEWLEXENVWITHNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CREATEASYNCGENERATOROBJ_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CREATEASYNCGENERATOROBJ(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ASYNCGENERATORRESOLVE_V8_V8_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            auto v2 = inst->GetSrcReg(2);
            enc->result_.emplace_back(pandasm::Create_ASYNCGENERATORRESOLVE(v0, v1, v2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLSPREAD_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_SUPERCALLSPREAD(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLARROWRANGE_IMM8_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_SUPERCALLARROWRANGE(imm0, imm1, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DEFINEGETTERSETTERBYVALUE_V8_V8_V8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            auto v2 = inst->GetSrcReg(2);
            auto v3 = inst->GetSrcReg(3);
            enc->result_.emplace_back(pandasm::Create_DEFINEGETTERSETTERBYVALUE(v0, v1, v2, v3));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DYNAMICIMPORT:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_DYNAMICIMPORT());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DEFINEMETHOD_IMM16_ID16_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_DEFINEMETHOD(imm0, bc_id0, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::RESUMEGENERATOR:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_RESUMEGENERATOR());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::GETRESUMEMODE:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_GETRESUMEMODE());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::GETTEMPLATEOBJECT_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_GETTEMPLATEOBJECT(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SUSPENDGENERATOR_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_SUSPENDGENERATOR(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONAWAITUNCAUGHT_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_ASYNCFUNCTIONAWAITUNCAUGHT(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SETOBJECTWITHPROTO_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_SETOBJECTWITHPROTO(imm0, v0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM8_V8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_STSUPERBYVALUE(imm0, v0, v1));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYVALUE_IMM16_V8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_STSUPERBYVALUE(imm0, v0, v1));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONRESOLVE_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_ASYNCFUNCTIONRESOLVE(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::ASYNCFUNCTIONREJECT_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_ASYNCFUNCTIONREJECT(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::COPYRESTARGS_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_COPYRESTARGS(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM8_ID16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_STSUPERBYNAME(imm0, bc_id0, v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STSUPERBYNAME_IMM16_ID16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_STSUPERBYNAME(imm0, bc_id0, v0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::SETGENERATORSTATE_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_SETGENERATORSTATE(imm0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::GETASYNCITERATOR_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_GETASYNCITERATOR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::LDPRIVATEPROPERTY_IMM8_IMM16_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_LDPRIVATEPROPERTY(imm0, imm1, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::STPRIVATEPROPERTY_IMM8_IMM16_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_STPRIVATEPROPERTY(imm0, imm1, imm2, v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::TESTIN_IMM8_IMM16_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            enc->result_.emplace_back(pandasm::Create_TESTIN(imm0, imm1, imm2));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DEFINEFIELDBYNAME_IMM8_ID16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_DEFINEFIELDBYNAME(imm0, bc_id0, v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::DEFINEPROPERTYBYNAME_IMM8_ID16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_DEFINEPROPERTYBYNAME(imm0, bc_id0, v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NOTIFYCONCURRENTRESULT_PREF_NONE:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_NOTIFYCONCURRENTRESULT());
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_CREATEOBJECTWITHEXCLUDEDKEYS_PREF_IMM16_V8_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_WIDE_CREATEOBJECTWITHEXCLUDEDKEYS(imm0, v0, v1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYVALUE_PREF_IMM8_V8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_DEFINEFIELDBYVALUE(imm0, v0, v1));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_NOTEXISTS_PREF_NONE:
       {
            enc->result_.emplace_back(pandasm::Create_THROW_NOTEXISTS());
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEFIELDBYINDEX_PREF_IMM8_IMM32_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_DEFINEFIELDBYINDEX(imm0, imm1, v0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENV_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_NEWLEXENV(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::THROW_PATTERNNONCOERCIBLE_PREF_NONE:
       {
            enc->result_.emplace_back(pandasm::Create_THROW_PATTERNNONCOERCIBLE());
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_TOPROPERTYKEY_PREF_NONE:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_TOPROPERTYKEY());
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_NEWLEXENVWITHNAME_PREF_IMM16_ID16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_WIDE_NEWLEXENVWITHNAME(imm0, bc_id0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::THROW_DELETESUPERPROPERTY_PREF_NONE:
       {
            enc->result_.emplace_back(pandasm::Create_THROW_DELETESUPERPROPERTY());
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CREATEPRIVATEPROPERTY_PREF_IMM16_ID16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_CREATEPRIVATEPROPERTY(imm0, bc_id0));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_CONSTASSIGNMENT_PREF_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_THROW_CONSTASSIGNMENT(v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINEPRIVATEPROPERTY_PREF_IMM8_IMM16_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm2 = static_cast<uint32_t>(inst->GetImms()[2]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_DEFINEPRIVATEPROPERTY(imm0, imm1, imm2, v0));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_IFNOTOBJECT_PREF_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_THROW_IFNOTOBJECT(v0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_CALLINIT_PREF_IMM8_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_CALLINIT(imm0, v0));
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLE_PREF_V8_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            auto v1 = inst->GetSrcReg(1);
            enc->result_.emplace_back(pandasm::Create_THROW_UNDEFINEDIFHOLE(v0, v1));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_DEFINESENDABLECLASS_PREF_IMM16_ID16_ID16_IMM16_V8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[1]);
            auto bc_id0 = enc->ir_interface_->GetMethodIdByOffset(ir_id0);
            ASSERT(inst->HasImms() && inst->GetImms().size() > 2); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id1 = static_cast<uint32_t>(inst->GetImms()[2]);
            auto bc_id1 = enc->ir_interface_->GetLiteralArrayByOffset(ir_id1);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 3); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm3 = static_cast<uint32_t>(inst->GetImms()[3]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_DEFINESENDABLECLASS(imm0, bc_id0, bc_id1, imm3, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLARROWRANGE_PREF_IMM16_V8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_WIDE_SUPERCALLARROWRANGE(imm0, v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_THROW_IFSUPERNOTCORRECTCALL(imm0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLECLASS_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDSENDABLECLASS(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_IFSUPERNOTCORRECTCALL_PREF_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_THROW_IFSUPERNOTCORRECTCALL(imm0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEEXTERNALMODULEVAR_PREF_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDSENDABLEEXTERNALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::THROW_UNDEFINEDIFHOLEWITHNAME_PREF_ID16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
            ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto ir_id0 = static_cast<uint32_t>(inst->GetImms()[0]);
            auto bc_id0 = enc->ir_interface_->GetStringIdByOffset(ir_id0);
            enc->result_.emplace_back(pandasm::Create_THROW_UNDEFINEDIFHOLEWITHNAME(bc_id0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEEXTERNALMODULEVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDELDSENDABLEEXTERNALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_NEWSENDABLEENV_PREF_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_NEWSENDABLEENV(imm0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_COPYRESTARGS_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_COPYRESTARGS(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDENEWSENDABLEENV_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDENEWSENDABLEENV(imm0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLEXVAR_PREF_IMM16_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_WIDE_LDLEXVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM4_IMM4:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_STSENDABLEVAR(imm0, imm1));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_STLEXVAR_PREF_IMM16_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_WIDE_STLEXVAR(imm0, imm1));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_STSENDABLEVAR_PREF_IMM8_IMM8:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_STSENDABLEVAR(imm0, imm1));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_GETMODULENAMESPACE_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_GETMODULENAMESPACE(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDESTSENDABLEVAR_PREF_IMM16_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDESTSENDABLEVAR(imm0, imm1));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_STMODULEVAR_PREF_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_STMODULEVAR(imm0));
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM4_IMM4:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDSENDABLEVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDLOCALMODULEVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_LDLOCALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDSENDABLEVAR_PREF_IMM8_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDSENDABLEVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDEXTERNALMODULEVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_LDEXTERNALMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }
       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDSENDABLEVAR_PREF_IMM16_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
           ASSERT(inst->HasImms() && inst->GetImms().size() > 1); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm1 = static_cast<uint32_t>(inst->GetImms()[1]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDELDSENDABLEVAR(imm0, imm1));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::WIDE_LDPATCHVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_LDPATCHVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


       case compiler::RuntimeInterface::IntrinsicId::WIDE_STPATCHVAR_PREF_IMM16:
       {
            auto acc_src = inst->GetSrcReg(inst->GetInputsCount() - 2);
            if (acc_src != compiler::ACC_REG_ID) {
                DoLda(acc_src, enc->result_);
            }
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_WIDE_STPATCHVAR(imm0));
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYMODULEVAR_PREF_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDLAZYMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYMODULEVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDELDLAZYMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_LDLAZYSENDABLEMODULEVAR_PREF_IMM8:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_LDLAZYSENDABLEMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_WIDELDLAZYSENDABLEMODULEVAR_PREF_IMM16:
       {
           ASSERT(inst->HasImms() && inst->GetImms().size() > 0); // NOLINTNEXTLINE(readability-container-size-empty)
            auto imm0 = static_cast<uint32_t>(inst->GetImms()[0]);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_WIDELDLAZYSENDABLEMODULEVAR(imm0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }

       case compiler::RuntimeInterface::IntrinsicId::CALLRUNTIME_SUPERCALLFORWARDALLARGS_PREF_V8:
       {
            auto v0 = inst->GetSrcReg(0);
            enc->result_.emplace_back(pandasm::Create_CALLRUNTIME_SUPERCALLFORWARDALLARGS(v0));
            auto acc_dst = inst->GetDstReg();
            if (acc_dst != compiler::ACC_REG_ID) {
                DoSta(inst->GetDstReg(), enc->result_);
            }
            break;
        }


        //////////////////////////////////////////////////////////////////////////////////////
        // hard trigger to be tested
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM8_IMM16:
        case compiler::RuntimeInterface::IntrinsicId::LDOBJBYINDEX_IMM16_IMM16:
        {
            panda::es2panda::ir::Expression* obj_expression = *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2);
            
            uint32_t imm;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_LDOBJBYINDEX_PREF_IMM32){
                imm = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                imm = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            panda::es2panda::ir::Expression* attr_expression = enc->get_literal_bynum(imm);

            auto objattrexpression = AllocNode<es2panda::ir::MemberExpression>(enc,
                                                        obj_expression,
                                                        attr_expression, 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

            enc->set_expression_by_register(inst, inst->GetDstReg(), objattrexpression);
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
                                                        *enc->get_expression_by_id(inst, 0),
                                                        enc->get_literal_bynum(imm), 
                                                        es2panda::ir::MemberExpression::MemberExpressionKind::PROPERTY_ACCESS, 
                                                        true, 
                                                        false);

             panda::es2panda::ir::Expression* assignexpression =   AllocNode<es2panda::ir::AssignmentExpression>(enc, 
                                                                            objattrexpression,
                                                                            *enc->get_expression_by_id(inst, inst->GetInputsCount() - 2),
                                                                            es2panda::lexer::TokenType::PUNCTUATOR_SUBSTITUTION
                                                                        ); 

            auto assignstatement = AllocNode<es2panda::ir::ExpressionStatement>(enc, 
                                                                                assignexpression);
            enc->add_insAst_to_blockstatemnt_by_inst(inst, assignstatement);
            break;        
        
        }
       
       case compiler::RuntimeInterface::IntrinsicId::SUPERCALLTHISRANGE_IMM8_IMM8_V8:
       case compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8:
       {
            panda::es2panda::ir::Expression* funname = enc->get_identifier_byname(new std::string("super"));
            enc->thisptr =  *enc->get_expression_by_id(inst, 0);

            uint32_t argsum;

            if(inst->GetIntrinsicId() == compiler::RuntimeInterface::IntrinsicId::WIDE_SUPERCALLTHISRANGE_PREF_IMM16_V8){
                argsum = static_cast<uint32_t>(inst->GetImms()[0]);
            }else{
                argsum = static_cast<uint32_t>(inst->GetImms()[1]);
            }

            ArenaVector<es2panda::ir::Expression *> arguments(enc->parser_program_->Allocator()->Adapter());
            for (uint32_t i = 0; i <= argsum; ++i) {
                arguments.push_back(*enc->get_expression_by_id(inst, i));
            }

            
            es2panda::ir::CallExpression* callarg0expression = AllocNode<es2panda::ir::CallExpression>(enc, 
                                                                                funname,
                                                                                std::move(arguments),
                                                                                nullptr,
                                                                                false
                                                                            );

            enc->set_expression_by_register(inst, inst->GetDstReg(), callarg0expression);
            break;
        }

        default:
            std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
            enc->success_ = false;
            LOG(ERROR,COMPILER) << "Unsupported ecma opcode";
    }
}
