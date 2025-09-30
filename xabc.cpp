
#define PANDA_USE_FUTEX 1
#define PANDA_TARGET_UNIX 1

#include "astgen.h"
#include "base.h"
#include "arkts.h"
#include "lexicalenv.h"
#include "modulevar.h"
#include "fundepscan.h"
#include "algos.h"

using namespace std;
using namespace panda;
using namespace bytecodeopt;

std::string inputFileName = "demo.abc";
std::string outputFileName = "arkdemo.ts";
std::string outputAstFileName = "arkdemo.ast";

template <typename T>
constexpr void RunOpts(compiler::Graph *graph)
{
    graph->RunPass<compiler::Cleanup>();
    graph->RunPass<T>();
}

template <typename First, typename Second, typename... Rest>
constexpr void RunOpts(compiler::Graph *graph)
{
    RunOpts<First>(graph);
    RunOpts<Second, Rest...>(graph);
}


static void SetCompilerOptions()
{
    compiler::options.SetCompilerUseSafepoint(false);
    compiler::options.SetCompilerSupportInitObjectInst(true);
    if (!compiler::options.WasSetCompilerMaxBytecodeSize()) {
        compiler::options.SetCompilerMaxBytecodeSize(MAX_BYTECODE_SIZE);
    }
}


static bool SkipFunction(const pandasm::Function &function, const std::string &func_name)
{
    if (panda::bytecodeopt::options.WasSetMethodRegex()) {
        static std::regex rgx(panda::bytecodeopt::options.GetMethodRegex());
        if (!std::regex_match(func_name, rgx)) {
            LOG(INFO, BYTECODE_OPTIMIZER) << "Skip Function " << func_name << ": Function's name doesn't match regex";
            return true;
        }
    }

    if (panda::bytecodeopt::options.IsSkipMethodsWithEh() && !function.catch_blocks.empty()) {
        LOG(INFO, BYTECODE_OPTIMIZER) << "Was not optimized " << func_name << ": Function has catch blocks";
        return true;
    }

    if ((function.regs_num + function.GetParamsNum()) > compiler::VIRTUAL_FRAME_SIZE) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Unable to optimize " << func_name
                                       << ": Function frame size is larger than allowed one";
        return true;
    }
    return false;
}

bool DecompileRunOptimizations(compiler::Graph *graph, BytecodeOptIrInterface *iface)
{
    constexpr int OPT_LEVEL_0 = 0;
    if (panda::bytecodeopt::options.GetOptLevel() == OPT_LEVEL_0) {
        return false;
    }

    graph->RunPass<compiler::Cleanup>();
    ASSERT(graph->IsDynamicMethod());
    if (compiler::options.IsCompilerBranchElimination()) {
       graph->RunPass<ConstantPropagation>(iface);
       RunOpts<compiler::BranchElimination>(graph);
    }
    
    RunOpts<compiler::ValNum, compiler::Lowering, compiler::MoveConstants>(graph);

    graph->RunPass<compiler::Cleanup>();
    graph->RunPass<RegAccAlloc>();

    graph->RunPass<compiler::Cleanup>();
    if (!RegAlloc(graph)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Failed compiler::RegAlloc";
        return false;
    }

    graph->RunPass<compiler::Cleanup>();
    if (!graph->RunPass<RegEncoder>()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Failed RegEncoder";
        return false;
    }

    return true;
}

bool DecompileFunction(pandasm::Program *prog, panda::es2panda::parser::Program *parser_program, 
                        BytecodeOptIrInterface *ir_interface,
                        const panda_file::MethodDataAccessor &mda, bool is_dynamic,
                        std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack, 
                        std::map<uint32_t, std::string*>* patchvarspace,
                        std::map<size_t, std::vector<std::string>>& index2namespaces, 
                        std::vector<std::string>& localnamespaces,
                        std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns,
                        std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> *method2scriptfunast,
                        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *>* ctor2classdeclast,
                        std::vector<uint32_t>* thisfuns,
                        std::map<uint32_t, panda::es2panda::ir::Expression*> *class2father,
                        std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>> *method2lexicalmap,
                        std::vector<LexicalEnvStack*>* globallexical_waitlist
                        )
{

    ArenaAllocator allocator {SpaceType::SPACE_TYPE_COMPILER};
    ArenaAllocator local_allocator {SpaceType::SPACE_TYPE_COMPILER, nullptr, true};

    SetCompilerOptions();

    auto func_name = ir_interface->GetMethodIdByOffset(mda.GetMethodId().GetOffset() );
    std::cout << std::endl << "[+] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Decompile "  << func_name << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< [+]" << std::endl << std::endl;

    auto it = prog->function_table.find(func_name);
    if (it == prog->function_table.end()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Cannot find function: " << func_name;
        return false;
    }

    LOG(INFO, BYTECODE_OPTIMIZER) << "Optimizing function: " << func_name;

    auto method_ptr = reinterpret_cast<compiler::RuntimeInterface::MethodPtr>(mda.GetMethodId().GetOffset());
    
    panda::BytecodeOptimizerRuntimeAdapter adapter(mda.GetPandaFile());
    auto graph = allocator.New<compiler::Graph>(&allocator, &local_allocator, Arch::NONE, method_ptr, &adapter, false,
                                                nullptr, is_dynamic, true);
    

    panda::pandasm::Function &function = it->second;

    if (SkipFunction(function, func_name)) {
        return false;
    }


    if ((graph == nullptr) || !graph->RunPass<panda::compiler::IrBuilder>()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": IR builder failed!";
        std::cout << "Optimizing " << func_name << ": IR builder failed!" << std::endl;
        return false;
    }

    if (graph->HasIrreducibleLoop()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Graph has irreducible loop!";
        std::cout << "Optimizing " << func_name << ": Graph has irreducible loop!" << std::endl;
        return false;
    }

    if (!DecompileRunOptimizations(graph, ir_interface)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Running optimizations failed!";
        std::cout << "Optimizing " << func_name << ": Running optimizations failed!" << std::endl;
        return false;
    }
    
    std::ofstream dump_out("logs/" + func_name+ ".ir");
    graph->Dump(&dump_out);

    
    std::string turefunname = extractTrueFunName(func_name);

    if (!graph->RunPass<AstGen>(&function, ir_interface, prog, parser_program, mda.GetMethodId().GetOffset(), method2lexicalenvstack, patchvarspace, std::ref(index2namespaces),
            std::ref(localnamespaces), std::ref(class2memberfuns), std::ref(method2scriptfunast), std::ref(ctor2classdeclast), std::ref(thisfuns), 
            std::ref(class2father), std::ref(method2lexicalmap), std::ref(globallexical_waitlist), turefunname)) {

        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Code generation failed!";

        std::cout << "Decompiling " << func_name << ": Code generation failed!" << std::endl;

        return false;
    }

    std::cout << std::endl << "[-] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Decompile "  << func_name << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< [-]" << std::endl << std::endl;

    return true;
}

void LogAst(panda::es2panda::parser::Program *parser_program, std::string outputFileName){
    std::cout << "[+] log raw ast start >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    std::string res = parser_program->Dump();
    //std::cout << res << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "can't open output file: " << outputFileName << std::endl;
    }else{
        outputFile << res;
        outputFile.close();
    }
    std::cout << "[-] log raw ast end >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
}

void LogArkTS2File(panda::es2panda::parser::Program *parser_program, std::string outputFileName){
    std::cout << "[+] log arkTS  start >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
    auto astsgen = panda::es2panda::ir::ArkTSGen(parser_program->Ast());
    
    std::cout << astsgen.Str() << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "can't open output file: " << outputFileName << std::endl;
    }else{
        outputFile << astsgen.Str();
        outputFile.close();
    }
    std::cout << "[-] log arkTS  end >>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
}


bool ScanFunDep(pandasm::Program *prog, panda::disasm::Disassembler& disasm,
                BytecodeOptIrInterface *ir_interface,
                std::vector<std::pair<uint32_t, uint32_t>>* depedges,
                std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns,
                std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>>* method2lexicalmap,
                std::vector<uint32_t>* thisfuns,
                const panda_file::MethodDataAccessor &mda, bool is_dynamic)
{

    ArenaAllocator allocator {SpaceType::SPACE_TYPE_COMPILER};
    ArenaAllocator local_allocator {SpaceType::SPACE_TYPE_COMPILER, nullptr, true};

    SetCompilerOptions();

    auto func_name = ir_interface->GetMethodIdByOffset(mda.GetMethodId().GetOffset() );
    std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>> "  << func_name << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    [[maybe_unused]] auto it = prog->function_table.find(func_name);
    if (it == prog->function_table.end()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Cannot find function: " << func_name;
        return false;
    }

    LOG(INFO, BYTECODE_OPTIMIZER) << "Optimizing function: " << func_name;

    auto method_ptr = reinterpret_cast<compiler::RuntimeInterface::MethodPtr>(mda.GetMethodId().GetOffset());
    
    panda::BytecodeOptimizerRuntimeAdapter adapter(mda.GetPandaFile());
    auto graph = allocator.New<compiler::Graph>(&allocator, &local_allocator, Arch::NONE, method_ptr, &adapter, false,
                                                nullptr, is_dynamic, true);
    

    panda::pandasm::Function &function = it->second;

    if (SkipFunction(function, func_name)) {
        return false;
    }
    
    if ((graph == nullptr) || !graph->RunPass<panda::compiler::IrBuilder>()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": IR builder failed!";
        std::cout << "Optimizing " << func_name << ": IR builder failed!" << std::endl;
        return false;
    }

    if (graph->HasIrreducibleLoop()) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Graph has irreducible loop!";
        std::cout << "Optimizing " << func_name << ": Graph has irreducible loop!" << std::endl;
        return false;
    }

    if (!DecompileRunOptimizations(graph, ir_interface)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Running optimizations failed!";
        std::cout << "Optimizing " << func_name << ": Running optimizations failed!" << std::endl;
        return false;
    }
    
    std::string turefunname = extractTrueFunName(func_name);
    if (!graph->RunPass<FunDepScan>(ir_interface, prog, std::ref(disasm), mda.GetMethodId().GetOffset(), turefunname, depedges, class2memberfuns, method2lexicalmap, thisfuns)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": FuncDep scanning failed!";

        std::cout << "FuncDep Scanning " << func_name << ": failed!" << std::endl;

        return false;
    }

    return true;
}

bool ConstructClasses(std::map<uint32_t, std::vector<uint32_t>> &class2memberfuns, panda::es2panda::parser::Program *parser_program,  BytecodeOptIrInterface *ir_interface,
        std::map<uint32_t, panda::es2panda::ir::Expression*> &class2father, std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> &method2scriptfunast,
        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *> &ctor2classdeclast
        ){

    for(const auto & pair : class2memberfuns){
        auto constructor_offset = pair.first;
        auto member_funcs = pair.second;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto constructor_offset_name = ir_interface->GetMethodIdByOffset(constructor_offset);
        panda::es2panda::util::StringView name_view1 = panda::es2panda::util::StringView(*(new std::string(extractTrueFunName(constructor_offset_name))));

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
            handleError("#DecompilePandaFile: find constructor function fail!");
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
                handleError("#DecompilePandaFile: find member function fail!");
            }

            auto funcExpr = parser_program->Allocator()->New<es2panda::ir::FunctionExpression>(func);

            panda::es2panda::util::StringView name_view3 = panda::es2panda::util::StringView(*(new std::string(extractTrueFunName(ir_interface->GetMethodIdByOffset(member_func_offset)))));
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



bool DecompilePandaFile(pandasm::Program *prog, BytecodeOptIrInterface *ir_interface,
                       const std::string &pfile_name, panda::disasm::Disassembler& disasm, bool is_dynamic)
{
    auto pfile = panda_file::OpenPandaFile(pfile_name);
    if (!pfile) {
        LOG(FATAL, BYTECODE_OPTIMIZER) << "Can not open binary file: " << pfile_name;
    }

    bool result = true;
    panda::es2panda::parser::Program *parser_program = new panda::es2panda::parser::Program(panda::es2panda::ScriptExtension::TS);
    
    ArenaVector<panda::es2panda::ir::Statement *> program_statements(parser_program->Allocator()->Adapter());
    auto program_ast = parser_program->Allocator()->New<panda::es2panda::ir::BlockStatement>(nullptr, std::move(program_statements));
    parser_program->SetAst(program_ast);

    std::map<uint32_t, LexicalEnvStack*> method2lexicalenvstack;

    std::map<size_t, std::vector<std::string>> index2importnamespaces; 

    std::map<uint32_t, std::vector<uint32_t>> class2memberfuns;
    std::map<uint32_t, panda::es2panda::ir::Expression*> class2father;

    std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> method2scriptfunast;

    std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *> ctor2classdeclast;

    std::vector<uint32_t> thisfuns; // all member functions(all classes)
    
    std::vector<std::string> localnamespaces; 

    std::map<uint32_t, std::map<uint32_t,  std::vector<uint32_t>>> method2lexicalmap;

    [[maybe_unused]] std::vector<LexicalEnvStack*> globallexical_waitlist;

    parseModuleVars(pfile, disasm, parser_program, index2importnamespaces, localnamespaces);

    for (uint32_t id : pfile->GetClasses()) {
        panda_file::File::EntityId record_id {id};
        if (pfile->IsExternal(record_id)) {
            continue;
        }

        panda_file::ClassDataAccessor cda {*pfile, record_id};
        std::cout << "classname: " << std::left << std::setw(40) <<   cda.GetName().data  << " , fileds: " << cda.GetFieldsNumber() << " , method: " << cda.GetMethodsNumber() << " interface: " << cda.GetIfacesNumber() << " superclass: " <<  cda.GetSuperClassId()   << std::endl;
    }
    
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    for (uint32_t id : pfile->GetClasses()) {
        
        panda_file::File::EntityId record_id {id};

        if (pfile->IsExternal(record_id)) {
            continue;
        }

        panda_file::ClassDataAccessor cda {*pfile, record_id};
        std::vector<std::pair<uint32_t, uint32_t>> depedges;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        cda.EnumerateMethods([prog, &disasm, ir_interface, is_dynamic, &result, &depedges, &class2memberfuns, &method2lexicalmap, &thisfuns](panda_file::MethodDataAccessor &mda){
            if (!mda.IsExternal()) {
                result = ScanFunDep(prog, disasm, ir_interface, &depedges, &class2memberfuns, &method2lexicalmap, &thisfuns, mda, is_dynamic) && result;
                if(!result){
                    handleError("#DecompilePandaFile: fun dep scan failed!");
                }
            }
        });
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto sorted_methodoffsets = topologicalSort(depedges);
        std::map<uint32_t, std::string*> patchvarspace;

        for(const auto & methodoffset : sorted_methodoffsets ){
            panda_file::MethodDataAccessor mda(*pfile, panda_file::File::EntityId(methodoffset));
            result = DecompileFunction(prog, parser_program, ir_interface, mda, is_dynamic, &method2lexicalenvstack, &patchvarspace, index2importnamespaces, localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast, &thisfuns, &class2father, &method2lexicalmap, &globallexical_waitlist) && result;
            
            if(!result){
                handleError("#DecompilePandaFile: decomiple case 1 failed!");
            }
        }

        
        cda.EnumerateMethods([prog, parser_program, ir_interface, is_dynamic, &result, &method2lexicalenvstack,  &patchvarspace, &index2importnamespaces, &localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast, &thisfuns, &class2father, &method2lexicalmap, &globallexical_waitlist, sorted_methodoffsets](panda_file::MethodDataAccessor &mda){           
            if (!mda.IsExternal() && std::find(sorted_methodoffsets.begin(), sorted_methodoffsets.end(), mda.GetMethodId().GetOffset()) == sorted_methodoffsets.end() ){
                result = DecompileFunction(prog, parser_program, ir_interface, mda, is_dynamic, &method2lexicalenvstack,  &patchvarspace, index2importnamespaces, localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast, &thisfuns, &class2father, &method2lexicalmap, &globallexical_waitlist) && result;
                if(!result){
                    handleError("#DecompilePandaFile: decomiple case 2 failed!");
                }
            }
        });
    } 

    std::cout <<  "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" << std::endl;

    ConstructClasses(class2memberfuns, parser_program, ir_interface, class2father, method2scriptfunast, ctor2classdeclast);

    std::cout <<  "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD" << std::endl;

    for (auto it = method2scriptfunast.rbegin(); it != method2scriptfunast.rend(); ++it) {
        auto funcDecl = parser_program->Allocator()->New<panda::es2panda::ir::FunctionDeclaration>(it->second);
        program_ast->AddStatementAtPos(program_statements.size(), funcDecl);
    }

    for (auto it = ctor2classdeclast.begin(); it != ctor2classdeclast.end(); ++it) {
        program_ast->AddStatementAtPos(program_statements.size(), it->second);
    }

    LogAst(parser_program, outputAstFileName);
    LogArkTS2File(parser_program, outputFileName);

    std::cout << "EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE" << std::endl;

    return result;
}

void construct_PandaFileToPandaAsmMaps(panda::disasm::Disassembler& disas, pandasm::AsmEmitter::PandaFileToPandaAsmMaps* maps){
    for (const auto &[offset, name_value] : disas.string_offset_to_name_) {
        maps->strings[offset.GetOffset()] = std::string(name_value);
    }

    for (const auto &[name_value, offset] : disas.method_name_to_id_) {
        maps->methods[offset.GetOffset()] = std::string(name_value);
    }
    
    for (const auto &[name_value, offset] : disas.record_name_to_id_) {
        maps->classes[offset.GetOffset()] = std::string(name_value);
    }


}

bool DecompileBytecode(const std::string &pandafile_name, panda::disasm::Disassembler& disasm, 
                       bool is_dynamic, bool has_memory_pool)
{

    if (!has_memory_pool) {
        PoolManager::Initialize(PoolType::MALLOC);
    }
    
    panda::pandasm::Program* prog = &disasm.prog_;
    pandasm::AsmEmitter::PandaFileToPandaAsmMaps maps;

    construct_PandaFileToPandaAsmMaps(disasm, &maps);

    auto ir_interface = BytecodeOptIrInterface(&maps, prog);

    auto res = DecompilePandaFile(prog, &ir_interface, pandafile_name, disasm, is_dynamic);
    
    if (!has_memory_pool) {
        PoolManager::Finalize();
    }

    return res;
}


int main(int argc, char* argv[]) {
    if (argc > 1) {
        inputFileName = argv[1]; 
    }
    if (argc > 2) {
        outputFileName = argv[2]; 
    }

    panda::disasm::Disassembler disasm {};
    disasm.Disassemble(inputFileName, false, false);
    disasm.CollectInfo();

    DecompileBytecode(inputFileName, disasm, true, false);

    return 0;
}
