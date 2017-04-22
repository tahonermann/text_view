# Copyright (c) 2017, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# This is a dummy package config file used to allow the find_package calls in
# the CMake files for the example utilities to succeed when being built from
# the source directory (as opposed to from the build or install directories).

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(text_view_FOUND GLOBAL PROPERTY text_view_FOUND)
if(text_view_FOUND)
  return()
endif()

set(text_view_FOUND 1)

set_property(GLOBAL PROPERTY text_view_FOUND ${text_view_FOUND})
