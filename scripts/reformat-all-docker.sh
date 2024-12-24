#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

docker run --rm -v ".:/code" --user "$(id -u):$(id -g)" silkeh/clang:17 /code/scripts/reformat-all.sh
