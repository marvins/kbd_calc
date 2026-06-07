#!/bin/bash
# Sync local code to Pi Zero and build remotely

set -e

REMOTE_HOST="marvin@pizero"
REMOTE_PATH="Projects/kbd_calc"
CLEAN_FLAG=""
VERBOSE_FLAG=""

while getopts ":cv" opt; do
    case "${opt}" in
        c) CLEAN_FLAG="-c" ;;
        v) VERBOSE_FLAG="-t" ;;
        \?) echo "Invalid option: -${OPTARG}" >&2; exit 1 ;;
    esac
done

echo "=== Syncing code to ${REMOTE_HOST}... ==="
rsync -avz \
    --exclude='build/' \
    --exclude='.git/' \
    --exclude='venv/' \
    --exclude='*.o' \
    --exclude='*.a' \
    --exclude='*.so' \
    ./ "${REMOTE_HOST}:${REMOTE_PATH}/"

echo ""
echo "=== Running remote build... ==="
ssh "${REMOTE_HOST}" "cd ${REMOTE_PATH} && ./scripts/build.sh ${CLEAN_FLAG} ${VERBOSE_FLAG} -j 1 -p ZERO -s off 2>&1 | tee build.log"

echo ""
echo "=== Build complete. Pulling updated logs... ==="
rsync -avz "${REMOTE_HOST}:${REMOTE_PATH}/build.log" ./

echo "Done!"
