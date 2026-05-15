#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

BUILD_TYPE="Debug"
CLEAN=false

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
  -h    Show this help message
  -c    Clean build directory before building
  -d    Debug build (default)
  -r    Release build

Examples:
  $(basename "$0")          # Debug build
  $(basename "$0") -r       # Release build
  $(basename "$0") -c -r    # Clean then release build
  $(basename "$0") -c       # Clean only (no build)
EOF
}

while getopts ":hcdr" opt; do
    case "${opt}" in
        h) usage; exit 0 ;;
        c) CLEAN=true ;;
        d) BUILD_TYPE="Debug" ;;
        r) BUILD_TYPE="Release" ;;
        :) echo "Error: option -${OPTARG} requires an argument." >&2; usage; exit 1 ;;
        \?) echo "Error: unknown option -${OPTARG}" >&2; usage; exit 1 ;;
    esac
done

if ${CLEAN}; then
    echo "Cleaning build directory: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
    if [[ "${OPTIND}" -gt "$#" ]] && ! ${CLEAN} || [[ $# -eq 0 ]] || [[ "$*" == *"-c"* && "$*" != *"-r"* && "$*" != *"-d"* ]]; then
        echo "Clean complete."
        exit 0
    fi
fi

echo "Build type : ${BUILD_TYPE}"
echo "Project dir: ${PROJECT_DIR}"
echo "Build dir  : ${BUILD_DIR}"
echo ""

cmake -S "${PROJECT_DIR}" -B "${BUILD_DIR}" \
    -DSIMULATOR=ON \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

cmake --build "${BUILD_DIR}" --parallel "$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)"

echo ""
echo "Build complete: ${BUILD_DIR}/calc_sim"
