#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

TARGET='all'
ARCHITECTURE='x86'
CONFIG='debug'

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

echo "Building config=${CONFIG} architecture=${ARCHITECTURE} target=${TARGET}"
make -C build -j$(nproc) config=${CONFIG}_${ARCHITECTURE} "${TARGET}"

