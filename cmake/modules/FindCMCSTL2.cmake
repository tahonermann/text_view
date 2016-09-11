# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(CMCSTL2_FOUND GLOBAL PROPERTY CMCSTL2_FOUND)
get_property(CMCSTL2_INCLUDE_DIRS GLOBAL PROPERTY CMCSTL2_INCLUDE_DIRS)
if(CMCSTL2_FOUND)
  return()
endif()

message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME}")

find_path(
  CMCSTL2_INCLUDE_DIRS "experimental/ranges/concepts"
  PATHS ENV CMCSTL2_INSTALL_PATH
  PATH_SUFFIXES "/include")

# find_path sets a cached variable, but find modules shouldn't do so according
# to the cmake-developer documentation.  So, set a normal variable and unset
# the cache variable.
set(CMCSTL2_INCLUDE_DIRS ${CMCSTL2_INCLUDE_DIRS})
unset(CMCSTL2_INCLUDE_DIRS CACHE)

if(CMCSTL2_INCLUDE_DIRS)
  set_property(GLOBAL PROPERTY CMCSTL2_INCLUDE_DIRS ${CMCSTL2_INCLUDE_DIRS})
  set(CMCSTL2_FOUND 1)
  set_property(GLOBAL PROPERTY CMCSTL2_FOUND ${CMCSTL2_FOUND})
endif()

if(CMCSTL2_FOUND)
  message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - found")
else()
  message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - not found")
  if(CMCSTL2_FIND_REQUIRED)
    message("Set the CMCSTL2_INSTALL_PATH environment variable to the"
            " location of the CMCSTL2 installation")
    message(FATAL_ERROR "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  elseif(NOT CMCSTL2_FIND_QUIETLY)
    message(WARNING "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  endif()
endif()
