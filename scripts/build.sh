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

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
  -h           Show this help message
  -c           Clean build directory, then build
  -d           Debug build (default)
  -r           Release build
  -t <target>  Target device: SDL or KN34 (default: SDL)
  -s <on|off>  Simulator mode: on or off (default: on)
  -j <jobs>    Parallel jobs (default: 1)

Examples:
  $(basename "$0")              # SDL simulator, debug build
  $(basename "$0") -t KN34      # KN34 target, simulator off
  $(basename "$0") -t SDL -r    # SDL simulator, release build
  $(basename "$0") -c -r         # Clean then release build
  $(basename "$0") -c            # Clean then build
  $(basename "$0") -j 4          # Use 4 parallel jobs
EOF
}

while getopts ":hcdrt:s:j:" opt; do
    case "${opt}" in
        h) usage; exit 0 ;;
        c) CLEAN=true ;;
        d) BUILD_TYPE="Debug" ;;
        r) BUILD_TYPE="Release" ;;
        t) TARGET_DEVICE="${OPTARG}" ;;
        s) SIMULATOR="${OPTARG}" ;;
        j) JOBS="${OPTARG}" ;;
        :) echo "Error: option -${OPTARG} requires an argument." >&2; usage; exit 1 ;;
        \?) echo "Error: unknown option -${OPTARG}" >&2; usage; exit 1 ;;
    esac
done

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

cmake --build "${BUILD_DIR}" --parallel "${JOBS}"

echo ""
if [[ "${SIMULATOR}" == "ON" ]]; then
    echo "Build complete: ${BUILD_DIR}/calc_sim"
else
    echo "Build complete: ${BUILD_DIR}/calc_firmware"
fi
