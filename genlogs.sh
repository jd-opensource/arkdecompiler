#!/bin/bash
MODULE_OPTION=$2

if [ "$MODULE_OPTION" = "module" ]; then
    ../out/x64.release/arkcompiler/ets_frontend/es2abc --module --dump-assembly  demo.ts  --output demo.abc > $1/demo.pa.raw
    ../out/x64.release/arkcompiler/ets_frontend/es2abc --module --dump-ast demo.ts --output demo.abc > $1/demo.ast
    ../out/x64.release/arkcompiler/runtime_core/ark_disasm demo.abc demo.pa
else
    ../out/x64.release/arkcompiler/ets_frontend/es2abc --dump-assembly  demo.ts  --output demo.abc > $1/demo.pa.raw
    ../out/x64.release/arkcompiler/ets_frontend/es2abc --dump-ast demo.ts --output demo.abc > $1/demo.ast
    ../out/x64.release/arkcompiler/runtime_core/ark_disasm demo.abc demo.pa
fi
