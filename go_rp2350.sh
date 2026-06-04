#!/usr/bin/env bash

set -e

echo "Building for RP2350..."
echo "Note: RP2350 requires ARM toolchain (arm-none-eabi-gcc) and Pico SDK"
echo "PICO_SDK_PATH: ${PICO_SDK_PATH:-NOT SET}"

# Use ARM cross-compiler via environment variables
export CC="${ARM_TOOLCHAIN_PATH:-/opt/homebrew/bin}/arm-none-eabi-gcc"
export CXX="${ARM_TOOLCHAIN_PATH:-/opt/homebrew/bin}/arm-none-eabi-g++"
export ASM="${ARM_TOOLCHAIN_PATH:-/opt/homebrew/bin}/arm-none-eabi-gcc"

PICO_TOOLCHAIN="${PICO_SDK_PATH}/cmake/preload/toolchains/pico_arm_cortex_m33_gcc.cmake"
./scripts/build.sh -c -j -t -p RP2350 -s off -T "${PICO_TOOLCHAIN}" > build.log 2>&1

if [ $? -eq 0 ]; then
    echo "Build successful. RP2350 firmware ready at build/calc_firmware"
    ls -la build/*.uf2 2>/dev/null && echo "UF2 files generated."
    echo "Note: RP2350 target is hardware-only, not testable via simulator."
else
    echo "Build failed. Check build.log for details."
    tail -30 build.log
    exit 1
fi
