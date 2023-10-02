#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

CLANG_FORMAT_BIN="${CLANG_FORMAT_BIN:-clang-format}"

find ./src ./test -iname '*.h' -o -iname '*.cpp' | xargs $CLANG_FORMAT_BIN -Werror -ferror-limit=1 --dry-run