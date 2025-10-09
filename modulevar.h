#ifndef DECOMPILER_MODULEVAR
#define DECOMPILER_MODULEVAR

#include "base.h"

using namespace panda;

std::string GetStringByOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset);

bool IsValidOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset);

void AddImportAst(panda::es2panda::parser::Program *parser_program, std::string imported_name, std::string local_name, std::string module_name);

void AddExportAstAll(panda::es2panda::parser::Program *parser_program,  std::string module_name);

void AddExportAstNamed(panda::es2panda::parser::Program *parser_program, std::string import_name, std::string export_name, std::string module_name);

void AddExportAst(panda::es2panda::parser::Program *parser_program,  std::string local_name, std::string export_name);

void GetModuleLiteralArray(std::unique_ptr<const panda_file::File>& file_, panda_file::File::EntityId &module_id, panda::disasm::Disassembler& disasm,
            panda::es2panda::parser::Program *parser_program, std::map<size_t, std::vector<std::string>>& index2namespaces, std::vector<std::string>& localnamespaces);

void ParseModuleVars(std::unique_ptr<const panda_file::File>& file_, panda::disasm::Disassembler& disasm, 
            panda::es2panda::parser::Program *parser_program, std::map<size_t, std::vector<std::string>>& index2namespaces, std::vector<std::string>& localnamespaces);

#endif