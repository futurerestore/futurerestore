#!/usr/bin/env bash
script_name=$(realpath "$0")
script_base=$(dirname "$script_name")
for file in $(find ${script_base}/dep_root/lib/pkgconfig -name "*.pc"); do export pkg=$(realpath $(dirname $(realpath "${file}"))/../../); pkg=$(echo $pkg | sed 's/\//\\\//g'); sed -i "s/^prefix=.*/prefix=${pkg}/g" $file; done