#include "modulevar.h"

std::string GetStringByOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset) 
{
    const auto sd = file_->GetStringData(panda_file::File::EntityId(offset));
    return std::string(utf::Mutf8AsCString(sd.data));
}

bool IsValidOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset) 
{
    return panda_file::File::EntityId(offset).IsValid() && offset < file_->GetHeader()->file_size;
}

void addimportast(panda::es2panda::parser::Program *parser_program, std::string imported_name, std::string local_name, std::string module_name){
    auto program_ast = parser_program->Ast();
    auto program_statements = program_ast->Statements();

    panda::es2panda::util::StringView imported_name_view = panda::es2panda::util::StringView(*new std::string(imported_name));
    auto imported_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(imported_name_view);

    panda::es2panda::util::StringView local_name_strview = panda::es2panda::util::StringView(*new std::string(local_name));
    [[maybe_unused]] auto local_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(local_name_strview);  

    
    auto importspefic = parser_program->Allocator()->New<panda::es2panda::ir::ImportSpecifier>(imported_nameid, local_nameid, false, false);

    ArenaVector<panda::es2panda::ir::AstNode *> specifiers(parser_program->Allocator()->Adapter());
    specifiers.push_back(importspefic);


    std::string* source_str_ptr = new std::string(module_name);
    es2panda::util::StringView literal_strview(*source_str_ptr);
    auto source = AllocNode<panda::es2panda::ir::StringLiteral>(parser_program, literal_strview);
    auto *importDeclaration = AllocNode<panda::es2panda::ir::ImportDeclaration>(parser_program, source, std::move(specifiers), nullptr, false, false);
    program_ast->AddStatementAtPos(program_statements.size(), importDeclaration);
}

void addexportast_all(panda::es2panda::parser::Program *parser_program,  std::string module_name){
    auto program_ast = parser_program->Ast();
    auto program_statements = program_ast->Statements();

    panda::es2panda::util::StringView local_name_strview = panda::es2panda::util::StringView(*new std::string("*"));
    [[maybe_unused]] auto local_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(local_name_strview);  

    
    std::string* source_str_ptr = new std::string(module_name);
    es2panda::util::StringView literal_strview(*source_str_ptr);
    auto source = AllocNode<panda::es2panda::ir::StringLiteral>(parser_program, literal_strview);

    auto *export_all = AllocNode<panda::es2panda::ir::ExportAllDeclaration>(parser_program, source, local_nameid, nullptr);
    program_ast->AddStatementAtPos(program_statements.size(), export_all);
}

void addexportast_named(panda::es2panda::parser::Program *parser_program, std::string import_name, std::string export_name, std::string module_name){
    auto program_ast = parser_program->Ast();
    auto program_statements = program_ast->Statements();

    panda::es2panda::util::StringView import_name_view = panda::es2panda::util::StringView(*new std::string(import_name));
    auto import_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(import_name_view);

    panda::es2panda::util::StringView export_name_strview = panda::es2panda::util::StringView(*new std::string(export_name));
    [[maybe_unused]] auto export_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(export_name_strview);  

    
    auto exportspefic = parser_program->Allocator()->New<panda::es2panda::ir::ExportSpecifier >(import_nameid, export_nameid, false);

    ArenaVector<panda::es2panda::ir::ExportSpecifier  *> specifiers(parser_program->Allocator()->Adapter());
    specifiers.push_back(exportspefic);


    std::string* source_str_ptr = new std::string(module_name);
    es2panda::util::StringView literal_strview(*source_str_ptr);
    auto source = AllocNode<panda::es2panda::ir::StringLiteral>(parser_program, literal_strview);
    auto *exportDeclaration = AllocNode<panda::es2panda::ir::ExportNamedDeclaration>(parser_program, source, std::move(specifiers), nullptr, false);
    program_ast->AddStatementAtPos(program_statements.size(), exportDeclaration);
}

void addexportast(panda::es2panda::parser::Program *parser_program,  std::string local_name, std::string export_name){
    auto program_ast = parser_program->Ast();
    auto program_statements = program_ast->Statements();

    panda::es2panda::util::StringView local_name_strview = panda::es2panda::util::StringView(*new std::string(local_name));
    [[maybe_unused]] auto local_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(local_name_strview);  

    panda::es2panda::util::StringView export_name_strview = panda::es2panda::util::StringView(*new std::string(export_name));
    [[maybe_unused]] auto export_nameid = parser_program->Allocator()->New<panda::es2panda::ir::Identifier>(export_name_strview);  

    auto *exportast = AllocNode<panda::es2panda::ir::ExportSpecifier >(parser_program, local_nameid, export_nameid, false);
    program_ast->AddStatementAtPos(program_statements.size(), exportast);

}

void GetModuleLiteralArray(std::unique_ptr<const panda_file::File>& file_, panda_file::File::EntityId &module_id, panda::disasm::Disassembler& disasm,
            panda::es2panda::parser::Program *parser_program, std::map<size_t, std::vector<std::string>> &index2namespaces, 
            std::vector<std::string>& localnamespaces)
{
    // addimportast(parser_program, "a", "b", "./module_foo1"); // mock refs

    std::map<size_t, std::string> index2importmodule;
    std::map<std::string, size_t> importmodule2index;

    std::map<size_t, std::map<std::string, std::string>> index2importmaps;
    std::vector<std::map<std::string, std::string>> exportmaps_arrays;

    panda_file::ModuleDataAccessor mda(*file_, module_id);
    const std::vector<uint32_t> &request_modules_offset = mda.getRequestModules();
    std::vector<std::string> module_literal_array;

    for (size_t index = 0; index < request_modules_offset.size(); ++index) {
        index2importmodule[index] = GetStringByOffset(file_, request_modules_offset[index]);
        importmodule2index[GetStringByOffset(file_, request_modules_offset[index])] = index;

        index2namespaces[index] = std::vector<std::string>();
    }

    mda.EnumerateModuleRecord([&](panda_file::ModuleTag tag, uint32_t export_name_offset, uint32_t request_module_idx,
                                  uint32_t import_name_offset, uint32_t local_name_offset) {

        std::map<std::string, std::string> curmaps;

        std::stringstream ss;
        if (tag == panda_file::ModuleTag::REGULAR_IMPORT ||
            tag == panda_file::ModuleTag::NAMESPACE_IMPORT || tag == panda_file::ModuleTag::LOCAL_EXPORT) {
            if (!disasm.IsValidOffset(local_name_offset)) {
                LOG(ERROR, DISASSEMBLER) << "Get invalid local name offset!" << std::endl;
                return;
            }
            curmaps["local_name"] = GetStringByOffset(file_, local_name_offset);

            ss << ", local_name: " << GetStringByOffset(file_, local_name_offset);
        }
        if (tag == panda_file::ModuleTag::LOCAL_EXPORT || tag == panda_file::ModuleTag::INDIRECT_EXPORT) {
            if (!IsValidOffset(file_, export_name_offset)) {
                LOG(ERROR, DISASSEMBLER) << "Get invalid export name offset!" << std::endl;
                return;
            }
            curmaps["export_name"] = GetStringByOffset(file_, export_name_offset);

            ss << ", export_name: " << GetStringByOffset(file_, export_name_offset);
        }
        if (tag == panda_file::ModuleTag::REGULAR_IMPORT || tag == panda_file::ModuleTag::INDIRECT_EXPORT) {
            if (!IsValidOffset(file_, import_name_offset)) {
                LOG(ERROR, DISASSEMBLER) << "Get invalid import name offset!" << std::endl;
                return;
            }
            curmaps["import_name"] = GetStringByOffset(file_, import_name_offset);

            ss << ", import_name: " << GetStringByOffset(file_, import_name_offset);
        }
        if (tag != panda_file::ModuleTag::LOCAL_EXPORT) {
            if (request_module_idx >= request_modules_offset.size() ||
                !IsValidOffset(file_, request_modules_offset[request_module_idx])) {
                LOG(ERROR, DISASSEMBLER) << "Get invalid request module offset!" << std::endl;
                return;
            }
            curmaps["module_request"] = GetStringByOffset(file_, request_modules_offset[request_module_idx]);

            ss << ", module_request: " << GetStringByOffset(file_, request_modules_offset[request_module_idx]);
        }


        if(tag == panda_file::ModuleTag::REGULAR_IMPORT  || tag == panda_file::ModuleTag::NAMESPACE_IMPORT){
            ss << ", IMPORT ";
            auto module_request_name = curmaps["module_request"];
            auto index = importmodule2index[module_request_name];

            index2importmaps[index] = curmaps;
            if(curmaps.count("import_name") > 0){
                addimportast(parser_program, curmaps["import_name"], curmaps["local_name"], curmaps["module_request"]);
            }else{
                addimportast(parser_program, "*", curmaps["local_name"], curmaps["module_request"]);
            }
            index2namespaces[importmodule2index[curmaps["module_request"]]].push_back(curmaps["local_name"]);
        }else{
            ss << ", EXPORT ";
            exportmaps_arrays.push_back(curmaps);
            // std::cout << curmaps.count("export_name") << " , " << curmaps.count("import_name") << " , " << curmaps.count("local_name") << " , " << curmaps.count("module_request") << std::endl;
            if(curmaps.count("export_name") == 0 && curmaps.count("local_name") == 0){
                // ExportAllDeclaration
                addexportast_all(parser_program, curmaps["module_request"]);

            }else if(curmaps.count("module_request") == 0){
                // ExportSpecifier
                addexportast(parser_program, curmaps["local_name"], curmaps["export_name"]);
                localnamespaces.push_back(curmaps["local_name"]);
            }else{
                // ExportNamedDeclaration
                addexportast_named(parser_program, curmaps["import_name"], curmaps["export_name"], curmaps["module_request"]);
                localnamespaces.push_back(curmaps["import_name"]);
            }
        }

        std::cout << ss.str() << std::endl;;
    });

}

void parseModuleVars(std::unique_ptr<const panda_file::File>& file_, panda::disasm::Disassembler& disasm, 
            panda::es2panda::parser::Program *parser_program, std::map<size_t, std::vector<std::string>>& index2namespaces, 
            std::vector<std::string>& localnamespaces){
    
    std::cout << "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH" << std::endl;
    panda::libpandafile::CollectUtil collect_util;

    std::unordered_set<uint32_t> literal_array_ids;
    collect_util.CollectLiteralArray(*file_, literal_array_ids);
    size_t index = 0;

    if (panda_file::ContainsLiteralArrayInHeader(file_->GetHeader()->version)) {
        const auto lit_arrays_id = file_->GetLiteralArraysId();

        panda_file::LiteralDataAccessor lda(*file_, lit_arrays_id);
        size_t num_litarrays = lda.GetLiteralNum();

        for (size_t index = 0; index < num_litarrays; index++) {
            
            auto id = lda.GetLiteralArrayId(index);
            if (disasm.module_request_phase_literals_.count(id.GetOffset())) {
                continue;
            }
            if (disasm.IsModuleLiteralOffset(id)) {
                GetModuleLiteralArray(file_, id, disasm, parser_program, index2namespaces, localnamespaces);
            }
        }
    }else{
        for (uint32_t literal_array_id : literal_array_ids) {
            panda_file::File::EntityId id {literal_array_id};

            if (disasm.IsModuleLiteralOffset(id)) {
                GetModuleLiteralArray(file_, id, disasm, parser_program, index2namespaces, localnamespaces);
            }
            
            index++;
        }
    }

    std::cout << "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE" << std::endl;
}