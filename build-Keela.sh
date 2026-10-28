#!/bin/bash

set -euo pipefail

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build

cd build

cpack -G NSIS