#!/bin/bash
../out/x64.debug/arkcompiler/ets_frontend/es2abc --module --dump-assembly  demo.ts  --output demo.abc > $1/demo.pa.raw
../out/x64.debug/arkcompiler/ets_frontend/es2abc --module --dump-ast demo.ts --output demo.abc > $1/demo.ast
../out/x64.debug/arkcompiler/runtime_core/ark_disasm demo.abc demo.pa
