#!/usr/bin/bash

set -o errexit
set -o nounset

build_dir=$(dirname $0)

cmake_preset="debug"
cmake_verbose=""

while getopts ":rv" opt; do
    case $opt in
        r)
            cmake_preset="release"
            ;;
        v)
            cmake_verbose="--verbose"
            ;;
        *)
            echo "$0: unknown option $opt"
            exit 1
            ;;
    esac
done

cd $build_dir
nice cmake --preset ${cmake_preset} ..
cd ${cmake_preset}
nice cmake --build ${cmake_verbose} .
