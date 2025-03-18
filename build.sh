../prebuilts/build-tools/linux-x86/bin/gn gen out --args='target_os="linux" target_cpu="x64" is_debug=false '
ninja -C out -w dupbuild=warn xabc
