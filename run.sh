#!/bin/bash
rm demo.abc

logdir=logs 

if [ "$1" = "module" ]; then
    ./genlogs.sh $logdir $1
elif [ "$1" = "hot" ]; then
    ./hot.sh
else
    ./genlogs.sh $logdir
fi

ninja -w dupbuild=warn xabc -C out
LD_LIBRARY_PATH=./out/arkcompiler/runtime_core:./out/thirdparty/zlib  ./out/arkcompiler/common/xabc
python3 scripts/removebc.py