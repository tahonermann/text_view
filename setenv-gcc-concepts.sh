#!/bin/sh

alias cdst="cd $PWD"

GCC_INSTALL_PATH="$HOME/products/gcc-c++-concepts"
export GCC_INSTALL_PATH

ORIGIN_INSTALL_PATH="$HOME/src/origin"
export ORIGIN_INSTALL_PATH

BOOST_INSTALL_PATH="$HOME/products/gcc-4.8.0-boost-1.53.0"
export BOOST_INSTALL_PATH

PATH="$GCC_INSTALL_PATH/bin:$PATH"
export PATH
