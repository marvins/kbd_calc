#!/usr/bin/env bash

set -e

CLEAN_BUILD=0
DEBUG_BUILD=0
DEBUG_LAYOUT=0
RUN_LLDB=0
SKIP_RUN=0
RUN_TESTS=0

while getopts "cdxlst" opt; do
    case $opt in
        c) CLEAN_BUILD=1 ;;
        d) DEBUG_BUILD=1 ;;
        x) DEBUG_LAYOUT=1 ;;
        l) RUN_LLDB=1 ;;
        s) SKIP_RUN=1 ;;
        t) RUN_TESTS=1 ;;
        \?)
            echo "Usage: $0 [-c] [-d] [-x] [-l] [-s] [-t]"
            echo "  -c  Clean build (clear old build)"
            echo "  -d  Debug build type"
            echo "  -x  Enable debug layout borders"
            echo "  -l  Run with lldb debugger"
            echo "  -s  Skip running simulator (build only)"
            echo "  -t  Run unit tests instead of simulator"
            exit 1
            ;;
    esac
done

echo "Building for SDL (TH33 macropad)..."

BUILD_ARGS="-j 4 -p SDL"
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
    echo "Build successful."
    if [ $RUN_TESTS -eq 1 ]; then
        echo "Running unit tests..."
        cd build
        ctest --output-on-failure
        cd ..
    elif [ $SKIP_RUN -eq 0 ]; then
        echo "Starting simulator..."
        if [ $RUN_LLDB -eq 1 ]; then
            lldb -- ./build/calc_sim --layout ./data/configs/th33 -v trace
        else
            ./build/calc_sim --layout ./data/configs/th33 -v trace
        fi
    else
        echo "Skipping simulator run (build only)."
    fi
else
    echo "Build failed. Check build.log for details."
    tail -20 build.log
    exit 1
fi

