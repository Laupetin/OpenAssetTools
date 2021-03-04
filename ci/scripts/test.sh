#!/bin/sh

# Go to repository root
cd "$(dirname "$0")/../../build/lib" || exit 2

./Release_x86/tests/ObjCommonTests
./Release_x86/tests/ZoneCodeGeneratorLibTests
./Release_x86/tests/ZoneCommonTests