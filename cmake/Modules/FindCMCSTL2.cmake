# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

if(CMCSTL2_FOUND)
  return()
endif()

message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME}")

find_path(CMCSTL2_INCLUDE_DIRS
          "experimental/ranges/concepts"
          PATHS ENV CMCSTL2_INSTALL_PATH
          PATH_SUFFIXES "/include")

if(CMCSTL2_INCLUDE_DIRS)
  set(CMCSTL2_FOUND 1 CACHE BOOL "Package ${CMAKE_FIND_PACKAGE_NAME} found")
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
