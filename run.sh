#!/bin/bash
rm demo.abc

logdir=logs 


if [ "$MODULE_OPTION" = "module" ]; then
    ./genlogs.sh $logdir $1
else
    ./genlogs.sh $logdir $1
fi

ninja -w dupbuild=warn xabc -C out
LD_LIBRARY_PATH=./out/arkcompiler/runtime_core:./out/thirdparty/zlib  ./out/arkcompiler/common/xabc
python3 scripts/removebc.py