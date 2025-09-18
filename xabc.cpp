
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

void BuildMapFromPcToIns(pandasm::Function &function, BytecodeOptIrInterface &ir_interface,
                         const compiler::Graph *graph, compiler::RuntimeInterface::MethodPtr method_ptr)
{
    CHECK_NOT_NULL(graph);
    function.local_variable_debug.clear();
    auto *pc_ins_map = ir_interface.GetPcInsMap();
    pc_ins_map->reserve(function.ins.size());
    auto instructions_buf = graph->GetRuntime()->GetMethodCode(method_ptr);
    compiler::BytecodeInstructions instructions(instructions_buf, graph->GetRuntime()->GetMethodCodeSize(method_ptr));
    compiler::BytecodeIterator insn_iter = instructions.begin();
    for (pandasm::Ins &ins : function.ins) {
        if (ins.opcode == pandasm::Opcode::INVALID) {
            continue;
        }
        pc_ins_map->emplace(instructions.GetPc(*insn_iter), &ins);
        ++insn_iter;
        if (insn_iter == instructions.end()) {
            break;
        }
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
                        const pandasm::AsmEmitter::PandaFileToPandaAsmMaps *maps,
                        const panda_file::MethodDataAccessor &mda, bool is_dynamic,
                        std::map<uint32_t, LexicalEnvStack*>* method2lexicalenvstack, 
                        std::map<uint32_t, std::string*>* patchvarspace,
                        std::map<size_t, std::vector<std::string>>& index2namespaces, 
                        std::vector<std::string>& localnamespaces,
                        std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns,
                        std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> *method2scriptfunast,
                        std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *>* ctor2classdeclast
                        )
{

    ArenaAllocator allocator {SpaceType::SPACE_TYPE_COMPILER};
    ArenaAllocator local_allocator {SpaceType::SPACE_TYPE_COMPILER, nullptr, true};

    SetCompilerOptions();

    auto ir_interface = BytecodeOptIrInterface(maps, prog);

    auto func_name = ir_interface.GetMethodIdByOffset(mda.GetMethodId().GetOffset() );
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

    // build map from pc to pandasm::ins (to re-build line-number info in BytecodeGen)
    BuildMapFromPcToIns(function, ir_interface, graph, method_ptr);
    
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

    if (!DecompileRunOptimizations(graph, &ir_interface)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Running optimizations failed!";
        std::cout << "Optimizing " << func_name << ": Running optimizations failed!" << std::endl;
        return false;
    }
    
    std::ofstream dump_out("logs/" + func_name+ ".ir");
    graph->Dump(&dump_out);

    
    std::string turefunname = extractTrueFunName(func_name);

    if (!graph->RunPass<AstGen>(&function, &ir_interface, prog, parser_program, mda.GetMethodId().GetOffset(), method2lexicalenvstack, patchvarspace, std::ref(index2namespaces), std::ref(localnamespaces), std::ref(class2memberfuns), std::ref(method2scriptfunast), std::ref(ctor2classdeclast), turefunname)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Code generation failed!";

        std::cout << "Decompiling " << func_name << ": Code generation failed!" << std::endl;

        return false;
    }

    std::cout << "Decompiled: " << func_name << std::endl;

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
                const pandasm::AsmEmitter::PandaFileToPandaAsmMaps *maps,
                std::vector<std::pair<uint32_t, uint32_t>>* depedges,
                std::map<uint32_t, std::vector<uint32_t>> *class2memberfuns,
                const panda_file::MethodDataAccessor &mda, bool is_dynamic)
{

    ArenaAllocator allocator {SpaceType::SPACE_TYPE_COMPILER};
    ArenaAllocator local_allocator {SpaceType::SPACE_TYPE_COMPILER, nullptr, true};

    SetCompilerOptions();

    auto ir_interface = BytecodeOptIrInterface(maps, prog);

    auto func_name = ir_interface.GetMethodIdByOffset(mda.GetMethodId().GetOffset() );
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

    BuildMapFromPcToIns(function, ir_interface, graph, method_ptr);
    
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

    if (!DecompileRunOptimizations(graph, &ir_interface)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": Running optimizations failed!";
        std::cout << "Optimizing " << func_name << ": Running optimizations failed!" << std::endl;
        return false;
    }
    
    std::string turefunname = extractTrueFunName(func_name);
    if (!graph->RunPass<FunDepScan>(&ir_interface, prog, std::ref(disasm), mda.GetMethodId().GetOffset(), turefunname, depedges, class2memberfuns)) {
        LOG(ERROR, BYTECODE_OPTIMIZER) << "Optimizing " << func_name << ": FuncDep scanning failed!";

        std::cout << "FuncDep Scanning " << func_name << ": failed!" << std::endl;

        return false;
    }

    return true;
}


bool DecompilePandaFile(pandasm::Program *prog, const pandasm::AsmEmitter::PandaFileToPandaAsmMaps *maps,
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

    std::map<uint32_t, panda::es2panda::ir::ScriptFunction *> method2scriptfunast;

    std::map<uint32_t, panda::es2panda::ir::ClassDeclaration *> ctor2classdeclast;

    std::vector<std::string> localnamespaces; 

    parseModuleVars(pfile, disasm, parser_program, index2importnamespaces, localnamespaces);

    for (uint32_t id : pfile->GetClasses()) {
        panda_file::File::EntityId record_id {id};
        if (pfile->IsExternal(record_id)) {
            continue;
        }

        panda_file::ClassDataAccessor cda {*pfile, record_id};
        std::cout << "classname: " << std::left << std::setw(40) <<   cda.GetName().data  << " , fileds: " << cda.GetFieldsNumber() << " , method: " << cda.GetMethodsNumber() << " interface: " << cda.GetIfacesNumber() << " superclass: " <<  cda.GetSuperClassId()   << std::endl;

    }

    for (uint32_t id : pfile->GetClasses()) {
        
        panda_file::File::EntityId record_id {id};

        if (pfile->IsExternal(record_id)) {
            continue;
        }

        std::cout << id << " XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << std::endl;

        panda_file::ClassDataAccessor cda {*pfile, record_id};
        std::vector<std::pair<uint32_t, uint32_t>> depedges;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        cda.EnumerateMethods([prog, &disasm, maps, is_dynamic, &result, &depedges, &class2memberfuns](panda_file::MethodDataAccessor &mda){
            if (!mda.IsExternal()) {
                result = ScanFunDep(prog, disasm, maps, &depedges, &class2memberfuns, mda, is_dynamic) && result;
                if(result){
                    std::cout << "<<<<<<<<<<<<<<<<<<<<   "<< "fun dep scan success! " << "  >>>>>>>>>>>>>>>>>>>>" << std::endl;
                }else{
                    std::cout << "<<<<<<<<<<<<<<<<<<<<   "<< "fun dep scan failed! " << "  >>>>>>>>>>>>>>>>>>>>" << std::endl;
                }
            }
        });
        
       
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto sorted_methodoffsets = topologicalSort(depedges);
        std::map<uint32_t, std::string*> patchvarspace;

        for(const auto & methodoffset : sorted_methodoffsets ){
            panda_file::MethodDataAccessor mda(*pfile, panda_file::File::EntityId(methodoffset));
            result = DecompileFunction(prog, parser_program, maps, mda, is_dynamic, &method2lexicalenvstack, &patchvarspace, index2importnamespaces, localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast) && result;
            if(result){
                LogAst(parser_program, outputAstFileName);
                LogArkTS2File(parser_program, outputFileName);
            }
        }

        int count = 0;
        std::cout <<  "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << std::endl;
        cda.EnumerateMethods([&count, prog, parser_program, maps, is_dynamic, &result, &method2lexicalenvstack,  &patchvarspace, &index2importnamespaces, &localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast, sorted_methodoffsets](panda_file::MethodDataAccessor &mda){
            count = count + 1;
            std::cout << "<<<<<<<<<<<<<<<<<<<<   "<< "enumerate method index: " << count << "  >>>>>>>>>>>>>>>>>>>>" << std::endl;
            if (!mda.IsExternal() && std::find(sorted_methodoffsets.begin(), sorted_methodoffsets.end(), mda.GetMethodId().GetOffset()) == sorted_methodoffsets.end() ){
                result = DecompileFunction(prog, parser_program, maps, mda, is_dynamic, &method2lexicalenvstack,  &patchvarspace, index2importnamespaces, localnamespaces, &class2memberfuns, &method2scriptfunast, &ctor2classdeclast) && result;
                if(result){
                    LogAst(parser_program, outputAstFileName);
                    LogArkTS2File(parser_program, outputFileName);
                }
            }
        });

        for (auto it = ctor2classdeclast.begin(); it != ctor2classdeclast.end(); ++it) {
            program_ast->AddStatementAtPos(program_statements.size(), it->second);
        }
        
        std::cout <<  "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" << std::endl;
       
    } 

    // LogAst(parser_program);
    // LogArkTS2File(parser_program, pfile_name);


    return result;
}


bool DecompileBytecode(pandasm::Program *prog, const pandasm::AsmEmitter::PandaFileToPandaAsmMaps *maps,
                      const std::string &pandafile_name, panda::disasm::Disassembler& disasm, 
                       bool is_dynamic, bool has_memory_pool)
{
    ASSERT(prog != nullptr);
    ASSERT(maps != nullptr);
    
    if (!has_memory_pool) {
        PoolManager::Initialize(PoolType::MALLOC);
    }
    
    auto res = DecompilePandaFile(prog, maps, pandafile_name, disasm, is_dynamic);
    
    if (!has_memory_pool) {
        PoolManager::Finalize();
    }

    return res;
}




void startDecompile(std::string &abc_file_name, panda::pandasm::Program &program, panda::pandasm::AsmEmitter::PandaFileToPandaAsmMaps& panda_file_to_asm_maps, panda::disasm::Disassembler& disasm) {
   
    DecompileBytecode(&program, &panda_file_to_asm_maps, abc_file_name, disasm, true, false);
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

    // const auto source = R"(
    //     .language ECMAScript
    //     .function any foo(any a0, any a1, any a2) {
    //         mov v0, a0
    //         mov v1, a1
    //         mov v2, a2
    //     try_begin:
    //         ldai 0x1
    //         trystglobalbyname 0x0, "a"
    //     try_end:
    //         jmp catch_end
    //     catch_begin:
    //         sta v4
    //         tryldglobalbyname 0x1, "a"
    //     catch_end:
    //         returnundefined
    //     }
    // )";

    // panda::pandasm::Parser parser;
    // auto res = parser.Parse(source);
    // auto &prog = res.Value();

    panda::pandasm::Program* program = &disasm.prog_;
    pandasm::AsmEmitter::PandaFileToPandaAsmMaps maps_;

    /////////////////////////////////////////////////////////
    construct_PandaFileToPandaAsmMaps(disasm, &maps_);

    startDecompile(inputFileName, *program, maps_, disasm);

    return 0;
}
