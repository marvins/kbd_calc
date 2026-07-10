#!/usr/bin/env bash

set -e

CLEAN_FLAG=""

while getopts ":c" opt; do
    case "${opt}" in
        c) CLEAN_FLAG="-c" ;;
        \?)
            echo "Usage: $0 [-c]"
            echo "  -c  Clean build"
            exit 1
            ;;
    esac
done

echo "Building USB HID Gadget firmware for Pico 2 (RP2350)..."
echo "Note: Requires ARM toolchain (arm-none-eabi-gcc) and Pico SDK"
echo "PICO_SDK_PATH:  ${PICO_SDK_PATH:-NOT SET}"

if [ -z "${PICO_SDK_PATH}" ]; then
    echo "Error: PICO_SDK_PATH environment variable not set."
    echo "Please set it to the path of the Raspberry Pi Pico SDK."
    exit 1
fi

# Build directory for USB gadget
USB_GADGET_BUILD_DIR="build_usb_gadget"

if [ "${CLEAN_FLAG}" == "-c" ]; then
    echo "Cleaning build directory: ${USB_GADGET_BUILD_DIR}"
    rm -rf "${USB_GADGET_BUILD_DIR}"
fi

# Create build directory
mkdir -p "${USB_GADGET_BUILD_DIR}"

# Configure with CMake
echo "Configuring..."
cmake -S src/usb_gadget -B "${USB_GADGET_BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DPICO_SDK_PATH="${PICO_SDK_PATH}" \
    > usb_gadget_build.log 2>&1

if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Check usb_gadget_build.log for details."
    tail -30 usb_gadget_build.log
    exit 1
fi

# Build
echo "Building..."
cmake --build "${USB_GADGET_BUILD_DIR}" --parallel $(nproc 2>/dev/null || echo 4) \
    >> usb_gadget_build.log 2>&1

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================" 
    echo "Build successful!"
    echo "========================================"
    echo ""
    echo "Firmware files:"
    ls -lh "${USB_GADGET_BUILD_DIR}"/*.uf2 2>/dev/null || echo "  (No UF2 files generated)"
    ls -lh "${USB_GADGET_BUILD_DIR}"/*.elf 2>/dev/null
    ls -lh "${USB_GADGET_BUILD_DIR}"/*.bin 2>/dev/null
    echo ""
    echo "To flash:"
    echo "  1. Hold BOOTSEL button on Pico 2"
    echo "  2. Connect USB cable"
    echo "  3. Copy UF2 file to mounted drive:"
    echo "     cp ${USB_GADGET_BUILD_DIR}/usb_hid_bridge.uf2 /media/RPI-RP2/"
    echo ""
else
    echo "Build failed. Check usb_gadget_build.log for details."
    tail -30 usb_gadget_build.log
    exit 1
fi
