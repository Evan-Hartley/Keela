#!/bin/bash

if [[ "$#" < 1 ]]; then
    echo ' '
    echo "For faster installation use bash build-Keela.sh <job-number>"
    echo "<job-number> max = # of computer processors"
    echo "<job-number> should be a multiple of 4"
    echo "Cancel current build using: Ctl+C"
    echo ' '
fi

set -euo pipefail

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

if [[ "$#" < 1 ]]; then
    cmake --build build
else
    cmake --build build -j${#1}
fi

cd build

cpack -G NSIS