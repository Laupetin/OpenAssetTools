#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

TARGET='all'
ARCHITECTURE='x86'
CONFIG='debug'
host_system="$(uname -s)"

if [ "${host_system}" = "Darwin" ]; then
    ARCHITECTURE='x64'
fi

for var in "$@"
do
    if [ "$var" == "debug" ] || [ "$var" == "release" ]; then
        CONFIG="$var"
    elif [ "$var" == "x86" ] || [ "$var" == "x64" ]; then
        ARCHITECTURE="$var"
    else
        TARGET="$var"
    fi
done

build_jobs=1

case "${host_system}" in
    Darwin)
        if [ "${ARCHITECTURE}" = "x86" ]; then
            echo "macOS builds support only the x64 architecture" >&2
            exit 2
        fi

        if detected_jobs="$(sysctl -n hw.ncpu 2>/dev/null)" && [ -n "${detected_jobs}" ]; then
            build_jobs="${detected_jobs}"
        fi
        ;;
    Linux)
        if detected_jobs="$(nproc 2>/dev/null)" && [ -n "${detected_jobs}" ]; then
            build_jobs="${detected_jobs}"
        fi
        ;;
esac

echo "Building config=${CONFIG} architecture=${ARCHITECTURE} target=${TARGET}"
make -C build -j"${build_jobs}" config=${CONFIG}_${ARCHITECTURE} "${TARGET}"
