#!/usr/bin/env bash

set -e

CLEAN_BUILD=0

while getopts "c" opt; do
    case $opt in
        c)
            CLEAN_BUILD=1
            ;;
        \?)
            echo "Usage: $0 [-c]"
            echo "  -c  Clean build (clear old build)"
            exit 1
            ;;
    esac
done

echo "Building for SDL (MF macropad)..."

BUILD_ARGS="-j -p SDL"
if [ $CLEAN_BUILD -eq 1 ]; then
    BUILD_ARGS="-c $BUILD_ARGS"
fi

./scripts/build.sh $BUILD_ARGS > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful. Starting simulator..."
    ./build/calc_sim --layout ./data/configs/mf -v trace
else
    echo "Build failed. Check build.log for details."
    tail -20 build.log
    exit 1
fi

