#ifndef DECOMPILER_BASE
#define DECOMPILER_BASE

#include "libpandabase/mem/arena_allocator.h"
#include "libpandabase/mem/pool_manager.h"

#include "libpandafile/class_data_accessor.h"
#include "libpandafile/class_data_accessor-inl.h"
#include "libpandafile/method_data_accessor.h"
#include "libpandafile/module_data_accessor-inl.h"

#include "libpandafile/file.h"
#include "libpandafile/util/collect_util.h"

#include "libpandabase/utils/logger.h"


/////////////////////////////////////////////////////////////
#include "compiler/optimizer/ir/graph.h"
#include "compiler/optimizer/pass.h"
#include "compiler/optimizer/ir_builder/ir_builder.h"
#include "compiler/optimizer/analysis/loop_analyzer.h"

#include "compiler/optimizer/optimizations/branch_elimination.h"
#include "compiler/optimizer/optimizations/cleanup.h"
#include "compiler/optimizer/optimizations/lowering.h"
#include "compiler/optimizer/optimizations/move_constants.h"
#include "compiler/optimizer/optimizations/regalloc/reg_alloc.h"
#include "compiler/optimizer/optimizations/vn.h"
#include "constant_propagation/constant_propagation.h"
#include "compiler/optimizer/ir/basicblock.h"
#include "compiler/optimizer/ir/graph_visitor.h"
#include "compiler/optimizer/ir/inst.h"
#include "compiler/optimizer/ir/runtime_interface.h"

/////////////////////////////////////////////////////////////

#include "bytecode_optimizer/ir_interface.h"
#include "bytecode_optimizer/runtime_adapter.h"
#include "bytecode_optimizer/bytecode_analysis_results.h"
#include "bytecode_optimizer/bytecodeopt_options.h"
#include "bytecode_optimizer/optimize_bytecode.h"
#include "bytecode_optimizer/reg_acc_alloc.h"
#include "bytecode_optimizer/reg_encoder.h"
#include "bytecode_optimizer/tagged_value.h"
#include "bytecode_optimizer/codegen.h"
#include "bytecode_optimizer/common.h"

/////////////////////////////////////////////////////////////
#include "assembler/annotation.h"
#include "assembler/assembly-function.h"
#include "assembler/assembly-ins.h"
#include "assembly-parser.h"
#include "assembler/assembly-parser.h"

#include "disassembler/disassembler.h"

#include "ast.h"

#include <string>
#include <typeinfo>
#include <sstream>
#include <variant>

#include <vector>
#include <memory>
#include <stdexcept>

void handleError(const std::string& errorMessage);

#endif