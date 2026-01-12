#ifndef DECOMPILER_CLASSCONSTRUCTION
#define DECOMPILER_CLASSCONSTRUCTION


#include "base.h"
using namespace panda;
using namespace bytecodeopt;

bool IsInstanceMethod(std::string func_name);

bool ConstructClasses(std::map<uint32_t, std::set<uint32_t>> &class2memberfuns, panda::es2panda::parser::Program *parser_program,  BytecodeOptIrInterface *ir_interface,
        std::map<uint32_t, panda::es2panda::ir::Expression*> &class2father, std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> &method2scriptfunast,
        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *> &ctor2classdeclast, std::map<std::string, std::string>& raw2newname
        );

#endif