#!/usr/bin/env bash

set -e

echo "Building for SDL (MF macropad)..."
./scripts/build.sh -c -j -p SDL > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful. Starting simulator..."
    ./build/calc_sim --layout ./data/configs/mf -v trace
else
    echo "Build failed. Check build.log for details."
    tail -20 build.log
    exit 1
fi

