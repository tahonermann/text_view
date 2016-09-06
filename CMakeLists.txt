# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(text_view CXX)

# Enable testing.
include(CTest)

# Check compiler requirements and set compiler dependent options.
if(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.2)
    message(FATAL_ERROR "GCC version 6.2 or later is required")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -fconcepts")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic")
else()
  message(FATAL_ERROR "Compilers other than GCC are not currently supported")
endif()

# Locate dependencies.
find_package(CMCSTL2 REQUIRED)

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
elseif(WIN32)
  set(TEXT_VIEW_DESTINATION_INCLUDE text_view/include)
  set(TEXT_VIEW_DESTINATION_SHARE text_view)
else()
  message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

# Install commands.
install(FILES LICENSE.txt README.md
        DESTINATION ${TEXT_VIEW_DESTINATION_SHARE})
install(DIRECTORY include/
        DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE}
        COMPONENT development)
install(DIRECTORY cmake
        DESTINATION ${TEXT_VIEW_DESTINATION_SHARE}
        COMPONENT development)
install(DIRECTORY examples/tv-dump
                  examples/tv-enumerate-utf8-code-points
                  examples/tv-find-utf8-multi-code-unit-code-point
        DESTINATION ${TEXT_VIEW_DESTINATION_SHARE}/examples
        COMPONENT examples
        PATTERN CMakeLists.txt EXCLUDE)
