#!/usr/bin/env bash

set -e

CLEAN_BUILD=0
DEBUG_BUILD=0
DEBUG_LAYOUT=0

while getopts "cdx" opt; do
    case $opt in
        c) CLEAN_BUILD=1 ;;
        d) DEBUG_BUILD=1 ;;
        x) DEBUG_LAYOUT=1 ;;
        \?)
            echo "Usage: $0 [-c] [-d] [-x]"
            echo "  -c  Clean build (clear old build)"
            echo "  -d  Debug build type"
            echo "  -x  Enable debug layout borders"
            exit 1
            ;;
    esac
done

echo "Building for PICOSDL..."

BUILD_ARGS="-j -p PICOSDL"
if [ $CLEAN_BUILD -eq 1 ]; then
    BUILD_ARGS="-c $BUILD_ARGS"
fi
if [ $DEBUG_BUILD -eq 1 ]; then
    BUILD_ARGS="$BUILD_ARGS -d"
fi
if [ $DEBUG_LAYOUT -eq 1 ]; then
    BUILD_ARGS="$BUILD_ARGS -x"
fi

./scripts/build.sh $BUILD_ARGS > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful. Starting simulator..."
    ./build/calc_sim --layout ./data/configs/picocalc -v trace
else
    echo "Build failed. Check build.log for details."
    tail -20 build.log
    exit 1
fi
