# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view CXX)

set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")

# Enable testing.
include(CTest)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fconcepts")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMCSTL2_DEFINITIONS}")

# Set compiler options.
include_directories(include)
include_directories(${CMCSTL2_INCLUDE_DIRS})

# Include sub-directories.
add_subdirectory(examples)
add_subdirectory(test)

# Determine the platform specific installation file layout.
if(UNIX)
  set(TEXT_VIEW_DESTINATION_INCLUDE include)
  set(TEXT_VIEW_DESTINATION_SHARE share/text_view)
  set(TEXT_VIEW_DESTINATION_CMAKE share/text_view/cmake)
elseif(WIN32)
  set(TEXT_VIEW_DESTINATION_INCLUDE text_view/include)
  set(TEXT_VIEW_DESTINATION_SHARE text_view)
  set(TEXT_VIEW_DESTINATION_CMAKE text_view/cmake)
else()
  message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
  cmake/text_view-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/text_view-config.cmake
  INSTALL_DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE})

# Install commands.
install(
  FILES LICENSE.txt README.md
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE})
install(
  DIRECTORY include/
  DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE}
  COMPONENT development)
install(
  DIRECTORY
    examples/tv-dump
    examples/tv-enumerate-utf8-code-points
    examples/tv-find-utf8-multi-code-unit-code-point
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE}/examples
  COMPONENT examples
  PATTERN CMakeLists.txt EXCLUDE)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/text_view-config.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}
  COMPONENT development)
install(
  FILES cmake/modules/FindCMCSTL2.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}/modules
  COMPONENT development)
