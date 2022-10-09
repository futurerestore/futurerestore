#!/usr/bin/env zsh

set -e
export WORKFLOW_ROOT=/Users/runner/work/futurerestore/futurerestore/.github/workflows
export DEP_ROOT=/Users/runner/work/futurerestore/futurerestore/dep_root
export BASE=/Users/runner/work/futurerestore/futurerestore/

cd ${WORKFLOW_ROOT}
curl -sO https://cdn.cryptiiiic.com/deps/static/Windows/x86_64/Windows_x86_64_Release_Latest.tar.zst
rm -rf ${DEP_ROOT}/{lib,include} || true
mkdir -p ${DEP_ROOT}/macOS_x86_64_Release ${DEP_ROOT}/macOS_x86_64_Debug ${DEP_ROOT}/macOS_arm64_Release ${DEP_ROOT}/macOS_arm64_Debug
gtar xf macOS_x86_64_Release_Latest.tar.zst -C ${DEP_ROOT}/macOS_x86_64_Release &
gtar xf macOS_x86_64_Debug_Latest.tar.zst -C ${DEP_ROOT}/macOS_x86_64_Debug &
gtar xf macOS_arm64_Release_Latest.tar.zst -C ${DEP_ROOT}/macOS_arm64_Release &
gtar xf macOS_arm64_Debug_Latest.tar.zst -C ${DEP_ROOT}/macOS_arm64_Debug &
wait
sudo mv /usr/local/bin{,1}
cd ${BASE}
git submodule update --init --recursive
cd ${BASE}/external/tsschecker
git submodule update --init --recursive
