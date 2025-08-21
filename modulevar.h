#include "ast.h"
#include "libpandabaseheaders.h"

using namespace panda;

std::string GetStringByOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset);

bool IsValidOffset(std::unique_ptr<const panda_file::File>& file_, uint32_t offset);

void addimportast(panda::es2panda::parser::Program *parser_program, std::string imported_name, std::string local_name, std::string module_name);

void addexportast_all(panda::es2panda::parser::Program *parser_program,  std::string module_name);

void addexportast_named(panda::es2panda::parser::Program *parser_program, std::string import_name, std::string export_name, std::string module_name);

void addexportast(panda::es2panda::parser::Program *parser_program,  std::string local_name, std::string export_name);

void GetModuleLiteralArray(std::unique_ptr<const panda_file::File>& file_, panda_file::File::EntityId &module_id, panda::disasm::Disassembler& disasm,
            panda::es2panda::parser::Program *parser_program);

void parseModuleVars(std::unique_ptr<const panda_file::File>& file_, panda::disasm::Disassembler& disasm, panda::es2panda::parser::Program *parser_program);