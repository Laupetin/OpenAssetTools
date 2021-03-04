#!/bin/bash

make -C build -j$(nproc) config=release_x86 all