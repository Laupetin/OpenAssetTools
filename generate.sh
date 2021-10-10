#!/bin/bash

# Go to repository root
cd "$(dirname "$0")" || exit 2

git submodule update --init --recursive
tools/premake5 $@ gmake2