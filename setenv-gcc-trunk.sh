#!/bin/sh

# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

alias cdtv="cd $PWD"

GCC_INSTALL_PATH="$HOME/products/gcc-trunk"
export GCC_INSTALL_PATH

ORIGIN_INSTALL_PATH="$HOME/products/origin-text_view"
export ORIGIN_INSTALL_PATH

PATH="$GCC_INSTALL_PATH/bin:$PATH"
export PATH
LD_LIBRARY_PATH="$GCC_INSTALL_PATH/lib32:$LD_LIBRARY_PATH"
LD_LIBRARY_PATH="$GCC_INSTALL_PATH/lib64:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH
