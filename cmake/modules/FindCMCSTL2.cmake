# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# find_package sets CMCSTL2_FOUND to "0" on each invocation.  A global property
# is used to retain state in order to avoid re-finding the package in the event
# of multiple find_package invocations.
get_property(CMCSTL2_FOUND GLOBAL PROPERTY CMCSTL2_FOUND)
if(CMCSTL2_FOUND)
  return()
endif()

message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME}")

find_path(
  CMCSTL2_INCLUDE_DIRS "experimental/ranges/concepts"
  PATHS ENV CMCSTL2_INSTALL_PATH
  PATH_SUFFIXES "/include")

if(CMCSTL2_INCLUDE_DIRS)
  set_property(GLOBAL PROPERTY CMCSTL2_FOUND 1)
  set(CMCSTL2_FOUND 1)
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
