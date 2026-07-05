#!/bin/bash
# Sync local code to Pi Zero and build remotely

set -e

REMOTE_HOST="marvin@pizero"
REMOTE_PATH="Projects/overboard"
CLEAN_FLAG=""
VERBOSE_FLAG=""
DEBUG_FLAG=""
DEBUG_LAYOUT_FLAG=""
SYNC_ONLY=false
PACKAGE_FLAG=false

while getopts ":cvdxnp" opt; do
    case "${opt}" in
        c) CLEAN_FLAG="-c" ;;
        v) VERBOSE_FLAG="-t" ;;
        d) DEBUG_FLAG="-d" ;;
        x) DEBUG_LAYOUT_FLAG="-x" ;;
        n) SYNC_ONLY=true ;;
        p) PACKAGE_FLAG=true ;;
        \?)
            echo "Usage: $0 [-c] [-d] [-n] [-p] [-v] [-x]"
            echo "  -c  Clean build"
            echo "  -d  Debug build type"
            echo "  -n  Sync only (no build)"
            echo "  -p  Create Debian package after build"
            echo "  -v  Verbose build output"
            echo "  -x  Enable debug layout borders"
            exit 1
            ;;
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

if ${SYNC_ONLY}; then
    echo "Sync complete (skipping build)."
    exit 0
fi

echo ""
echo "=== Running remote build... ==="
ssh "${REMOTE_HOST}" "cd ${REMOTE_PATH} && ./scripts/build.sh ${CLEAN_FLAG} ${DEBUG_FLAG} ${VERBOSE_FLAG} ${DEBUG_LAYOUT_FLAG} -j 1 -p ZERO -s off 2>&1 | tee build.log"

echo ""
echo "=== Build complete. Pulling updated logs... ==="
rsync -avz "${REMOTE_HOST}:${REMOTE_PATH}/build.log" ./

if ${PACKAGE_FLAG}; then
    echo ""
    echo "=== Creating Debian package... ==="
    ssh "${REMOTE_HOST}" "cd ${REMOTE_PATH} && cmake --install build --prefix /tmp/overboard-staging && cpack --config build/CPackConfig.cmake"
    echo ""
    echo "=== Pulling package... ==="
    rsync -avz "${REMOTE_HOST}:${REMOTE_PATH}/overboard-*.deb" ./
    echo "Package created: overboard-*.deb"
fi

echo "Done!"
