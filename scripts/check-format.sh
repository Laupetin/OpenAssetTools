#!/bin/bash

# Go to repository root
cd "$(dirname "$0")/.." || exit 2

find ./src ./test -iname '*.h' -o -iname '*.cpp' | xargs clang-format -Werror -ferror-limit=1 --dry-run