#include "classconstruction.h"

bool ConstructClasses(std::map<uint32_t, std::set<uint32_t>> &class2memberfuns, panda::es2panda::parser::Program *parser_program,  BytecodeOptIrInterface *ir_interface,
        std::map<uint32_t, panda::es2panda::ir::Expression*> &class2father, std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> &method2scriptfunast,
        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *> &ctor2classdeclast, std::map<std::string, std::string>& raw2newname
        ){

    for(const auto & pair : class2memberfuns){
        auto constructor_offset = pair.first;

        std::cout << constructor_offset << std::endl;

        auto member_funcs = pair.second;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto constructor_offset_name = RemoveArgumentsOfFunc(ir_interface->GetMethodIdByOffset(constructor_offset));
        std::string newname_constructor_offset_name;

        if(raw2newname.find(constructor_offset_name) != raw2newname.end()){
            newname_constructor_offset_name =  raw2newname[constructor_offset_name];
        }else{
            HandleError("#ConstructClasses: find constructor_offset_name newname error");
        }
       
        panda::es2panda::util::StringView name_view1 = panda::es2panda::util::StringView(*(new std::string(RemoveArgumentsOfFunc(newname_constructor_offset_name))));

        auto identNode =  parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(name_view1);

        ArenaVector<es2panda::ir::TSClassImplements *> implements(parser_program->Allocator()->Adapter());
        ArenaVector<es2panda::ir::TSIndexSignature *> indexSignatures(parser_program->Allocator()->Adapter());
        ArenaVector<es2panda::ir::Statement *> properties(parser_program->Allocator()->Adapter());
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        ArenaVector<es2panda::ir::Decorator *> decorators(parser_program->Allocator()->Adapter());
        ArenaVector<es2panda::ir::Annotation *> annotations(parser_program->Allocator()->Adapter());
        ArenaVector<es2panda::ir::ParamDecorators> paramDecorators(parser_program->Allocator()->Adapter());

        panda::es2panda::util::StringView name_view2 = panda::es2panda::util::StringView("constructor");
        auto keyNode = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(name_view2);

        auto func = method2scriptfunast[constructor_offset];
        method2scriptfunast.erase(constructor_offset);

        if(func == nullptr){
            HandleError("#DecompilePandaFile: find constructor function fail!");
        }

        auto funcExpr = parser_program->Allocator()->New<panda::es2panda::ir::FunctionExpression>(func);


        auto ctor = parser_program->Allocator()->New<es2panda::ir::MethodDefinition>(es2panda::ir::MethodDefinitionKind::CONSTRUCTOR, keyNode, funcExpr,
                                                        es2panda::ir::ModifierFlags::PUBLIC, parser_program->Allocator(), 
                                                    std::move(decorators), std::move(annotations), 
                                                    std::move(paramDecorators), false);

        
        ///////////////////////////////////////////////////////////////////////////////////////////////

        auto father = class2father[constructor_offset];
        auto classDefinition = parser_program->Allocator()->New<es2panda::ir::ClassDefinition>(
                            nullptr, identNode, nullptr, nullptr, std::move(implements), ctor, nullptr,
                            nullptr, father, std::move(properties), std::move(indexSignatures), false, false);
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (const auto& member_func_offset : member_funcs) {
            
            auto func = method2scriptfunast[member_func_offset];
            method2scriptfunast.erase(member_func_offset);

            if(func == nullptr){
                std::cout << "member function offset: " << member_func_offset << std::endl;
                HandleError("#ConstructClasses: find member function fail!");
            }

            auto funcExpr = parser_program->Allocator()->New<es2panda::ir::FunctionExpression>(func);
            auto raw_member_name  = RemoveArgumentsOfFunc(ir_interface->GetMethodIdByOffset(member_func_offset));
            std::string new_member_name;
            if(raw2newname.find(raw_member_name) != raw2newname.end()){
                new_member_name =  raw2newname[raw_member_name];
            }else{
                HandleError("#ConstructClasses: find new_member_name newname error");
            }

            panda::es2panda::util::StringView name_view3 = panda::es2panda::util::StringView(*(new std::string(new_member_name)));
            auto keyNode = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(name_view3);

            ArenaVector<es2panda::ir::Decorator *> decorators(parser_program->Allocator()->Adapter());
            ArenaVector<es2panda::ir::Annotation *> annotations(parser_program->Allocator()->Adapter());
            ArenaVector<es2panda::ir::ParamDecorators> paramDecorators(parser_program->Allocator()->Adapter());

            
            auto method = parser_program->Allocator()->New<es2panda::ir::MethodDefinition>(es2panda::ir::MethodDefinitionKind::METHOD, keyNode, funcExpr,
                                                            es2panda::ir::ModifierFlags::PUBLIC, parser_program->Allocator(), 
                                                        std::move(decorators), std::move(annotations), 
                                                        std::move(paramDecorators), false);
            classDefinition->AddToBody(method);

        }
  
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ArenaVector<es2panda::ir::Decorator *> decorators1(parser_program->Allocator()->Adapter());
        ArenaVector<es2panda::ir::Annotation *> annotations1(parser_program->Allocator()->Adapter());

       
        auto *classDecl = parser_program->Allocator()->New<es2panda::ir::ClassDeclaration>(classDefinition, 
                                                std::move(decorators1), std::move(annotations1), false);
        
        ctor2classdeclast[constructor_offset] = classDecl;
    }
    return true;
}
