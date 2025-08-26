../out/x64.debug/arkcompiler/ets_frontend/es2abc --module --dump-symbol-table base.map --output  1.abc 1.ts
../out/x64.debug/arkcompiler/ets_frontend/es2abc --module --hot-reload  --input-symbol-table base.map --output demo.abc 2.ts
../out/x64.debug/arkcompiler/runtime_core/ark_disasm demo.abc demo.pa
rm 1.abc