#!/usr/bin/env bash

set -e
export WORKFLOW_ROOT=/d/a/futurerestore/futurerestore/.github/workflows
export DEP_ROOT=/d/a/futurerestore/futurerestore/dep_root
export BASE=/d/a/futurerestore/futurerestore

cd ${WORKFLOW_ROOT}
curl -sO https://cdn.cryptiiiic.com/deps/static/Windows/x86_64/Windows_x86_64_Release_Latest.tar.zst
rm -rf ${DEP_ROOT}/{lib,include} || true
mkdir -p ${DEP_ROOT}/Windows_x86_64_Release
tar xf Windows_x86_64_Release_Latest.tar.zst -C ${DEP_ROOT}/Windows_x86_64_Release
cd ${BASE}
git submodule update --init --recursive
cd ${BASE}/external/tsschecker
git submodule update --init --recursive
