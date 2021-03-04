#!/bin/sh

# Go to repository root
cd "$(dirname "$0")/../.." || exit 2

docker run --rm -v "$(pwd):/mnt/oat" oat /bin/bash -c "/mnt/oat/generate.sh && /mnt/oat/build-linux.sh"