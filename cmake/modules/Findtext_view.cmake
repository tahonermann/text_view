# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(text_view_FOUND GLOBAL PROPERTY text_view_FOUND)
get_property(text_view_COMPILE_OPTIONS GLOBAL PROPERTY text_view_COMPILE_OPTIONS)
get_property(text_view_DEFINITIONS GLOBAL PROPERTY text_view_DEFINITIONS)
get_property(text_view_INCLUDE_DIRS GLOBAL PROPERTY text_view_INCLUDE_DIRS)
if(text_view_FOUND)
  return()
endif()

# Check compiler requirements and set compiler dependent options.
if(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.2)
    message(FATAL_ERROR "GCC version 6.2 or later is required")
  endif()
else()
  message(FATAL_ERROR "Compilers other than GCC are not currently supported")
endif()

# Locate dependencies.  Prefer config mode, but fall back to module mode if
# a package isn't found.
find_package(CMCSTL2 QUIET NO_MODULE)
if(NOT CMCSTL2_FOUND)
  find_package(CMCSTL2 MODULE REQUIRED)
endif()

# FIXME: Use of the CMake compiler features support would be more appropriate
# FIXME: here, but support for C++17 and Concepts has not yet been added.
set(text_view_COMPILE_OPTIONS
      ${CMCSTL2_COMPILE_OPTIONS}
      -std=c++17 -fconcepts
      -Wall -Werror -Wpedantic)
set_property(GLOBAL PROPERTY text_view_COMPILE_OPTIONS ${text_view_COMPILE_OPTIONS})

set(text_view_DEFINITIONS
      ${CMCSTL2_DEFINITIONS})
set_property(GLOBAL PROPERTY text_view_DEFINITIONS ${text_view_DEFINITIONS})

set(text_view_INCLUDE_DIR
      ${CMAKE_CURRENT_LIST_DIR}/../../include)
get_filename_component(
  text_view_INCLUDE_DIR
  ${text_view_INCLUDE_DIR}
  REALPATH)
set(text_view_INCLUDE_DIRS
      ${CMCSTL2_INCLUDE_DIRS}
      ${text_view_INCLUDE_DIR})
unset(text_view_INCLUDE_DIR)
set_property(GLOBAL PROPERTY text_view_INCLUDE_DIRS ${text_view_INCLUDE_DIRS})

set(text_view_FOUND 1)
set_property(GLOBAL PROPERTY text_view_FOUND ${text_view_FOUND})
