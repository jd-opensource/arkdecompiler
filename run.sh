#!/bin/bash
logdir=logs 
./genlogs.sh $logdir
ninja -w dupbuild=warn xabc -C out 
LD_LIBRARY_PATH=./out/arkcompiler/runtime_core:./out/thirdparty/zlib  ./out/arkcompiler/common/xabc
