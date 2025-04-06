#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

echo "Start building with $(nproc) threads"
make -C build -j$(nproc) config=release_x64 all
