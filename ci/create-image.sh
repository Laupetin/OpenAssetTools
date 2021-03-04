#!/bin/sh

# Go to repository root
cd "$(dirname "$0")" || exit 2

docker build -t oat .