#!/bin/sh

GCC_INSTALL_PATH="$HOME/products/gcc-c++-concepts"
export GCC_INSTALL_PATH

ORIGIN_INSTALL_PATH="$HOME/src/origin"
export ORIGIN_INSTALL_PATH

PATH="$GCC_INSTALL_PATH/bin:$PATH"
export PATH
