#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

BUILD_TYPE="Debug"
CLEAN=false
TARGET_DEVICE="SDL"
SIMULATOR=ON
JOBS="1"
VERBOSE=false

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
  -h           Show this help message
  -c           Clean build directory, then build
  -d           Debug build (default)
  -r           Release build
  -p <target>  Hardware target: SDL, PICOSDL, or RP2350 (default: SDL)
  -s <on|off>  Simulator mode: on or off (default: on)
  -j <jobs>    Parallel jobs (default: 1)
  -t           Trace/verbose build output

Examples:
  $(basename "$0")              # SDL simulator, debug build
  $(basename "$0") -p RP2350    # RP2350 target, simulator off
  $(basename "$0") -p PICOSDL   # PicoCalc SDL target, no keyboard UI
  $(basename "$0") -p SDL -r    # SDL simulator, release build
  $(basename "$0") -c -r         # Clean then release build
  $(basename "$0") -c            # Clean then build
  $(basename "$0") -j 4          # Use 4 parallel jobs
EOF
}

while getopts ":hcdrp:s:j:t" opt; do
    case "${opt}" in
        h) usage; exit 0 ;;
        c) CLEAN=true ;;
        d) BUILD_TYPE="Debug" ;;
        r) BUILD_TYPE="Release" ;;
        p) TARGET_DEVICE="${OPTARG}" ;;
        s) SIMULATOR="${OPTARG}" ;;
        j) JOBS="${OPTARG}" ;;
        t) VERBOSE=true ;;
        :) echo "Error: option -${OPTARG} requires an argument." >&2; usage; exit 1 ;;
        \?) echo "Error: unknown option -${OPTARG}" >&2; usage; exit 1 ;;
    esac
done

# Validate target device
case "${TARGET_DEVICE}" in
    SDL|PICOSDL|RP2350|PICOCALC) ;;
    *) echo "Error: unknown target '${TARGET_DEVICE}'. Valid targets: SDL, PICOSDL, RP2350, PICOCALC" >&2; usage; exit 1 ;;
esac

# Normalize simulator flag
case "${SIMULATOR}" in
    on|ON|On|1|true|TRUE|True) SIMULATOR=ON ;;
    off|OFF|Off|0|false|FALSE|False) SIMULATOR=OFF ;;
    *) echo "Error: -s requires 'on' or 'off'" >&2; usage; exit 1 ;;
esac

if ${CLEAN}; then
    echo "Cleaning build directory: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
    echo "Clean complete."
fi

echo "Build type    : ${BUILD_TYPE}"
echo "Target device : ${TARGET_DEVICE}"
echo "Simulator     : ${SIMULATOR}"
echo "Project dir   : ${PROJECT_DIR}"
echo "Build dir     : ${BUILD_DIR}"
echo ""

cmake -S "${PROJECT_DIR}" -B "${BUILD_DIR}" \
    -DTARGET_DEVICE="${TARGET_DEVICE}" \
    -DSIMULATOR="${SIMULATOR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

if ${VERBOSE}; then
    cmake --build "${BUILD_DIR}" --parallel "${JOBS}" --verbose
else
    cmake --build "${BUILD_DIR}" --parallel "${JOBS}"
fi

echo ""
case "${TARGET_DEVICE}" in
    SDL|PICOSDL)
        echo "Build complete: ${BUILD_DIR}/calc_sim" ;;
    *)
        echo "Build complete: ${BUILD_DIR}/calc_firmware" ;;
esac
