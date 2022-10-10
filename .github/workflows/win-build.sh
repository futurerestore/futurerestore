#!/usr/bin/env bash

set -e
export TMPDIR=/tmp
export WORKFLOW_ROOT=/d/a/futurerestore/futurerestore/.github/workflows
export DEP_ROOT=/d/a/futurerestore/futurerestore/dep_root
export BASE=/d/a/futurerestore/futurerestore

cd ${BASE}
ln -sf ${DEP_ROOT}/Windows_x86_64_Release/{lib/,include/}  ${DEP_ROOT}/
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CROSSCOMPILING=false -DCMAKE_INSTALL_PREFIX=/tmp/out -DBUILD_SHARED_LIBS=0 -Wno-dev -DCMAKE_MAKE_PROGRAM=$(which make) -DCMAKE_C_COMPILER=$(which clang) -DCMAKE_MESSAGE_LOG_LEVEL="WARNING" -DCMAKE_CXX_COMPILER=$(which clang++) -G "CodeBlocks - Unix Makefiles" -S ./ -B cmake-build-release-x86_64 -DARCH=x86_64 -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_LINKER=ld.lld -DCMAKE_C_FLAGS="-I/clang64/include" -DCMAKE_CXX_FLAGS="-I/clang64/include" -DNO_PKGCFG=1
make -j4 -l4 -C cmake-build-release-x86_64

#llvm-strip -s cmake-build-release-x86_64/src/futurerestore
