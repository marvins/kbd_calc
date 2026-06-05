#!/usr/bin/env bash

set -e

echo "Building for Pi Zero (DRM/KMS)..."
./scripts/build.sh -c -j -p ZERO > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful."
    echo "Binary: ./build/calc_app"
    echo ""
    echo "To run on Pi Zero:"
    echo "  1. Copy build/calc_app to the Pi Zero"
    echo "  2. Ensure keyboard.json is in the same directory"
    echo "  3. Run: ./calc_app --layout ./data/configs/mf -v trace"
else
    echo "Build failed. Check build.log for details."
    tail -20 build.log
    exit 1
fi
