# Copyright (c) 2017, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view CXX)

# Enable testing.
include(CTest)

set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH}
      ${CMAKE_CURRENT_LIST_DIR}/cmake/modules/)

# Locate dependencies.  Prefer config mode for CMCSTL2, but fall back to module
# mode if it isn't found.
find_package(CMCSTL2 QUIET NO_MODULE)
if(NOT CMCSTL2_FOUND)
  find_package(CMCSTL2 MODULE REQUIRED)
endif()

# Set variables used to set compiler options.
set(text_view_COMPILE_OPTIONS
      -Wall -Werror -Wpedantic)
set(text_view_DEFINITIONS)
set(text_view_INCLUDE_DIR
      ${CMAKE_CURRENT_LIST_DIR}/include)
get_filename_component(
  text_view_INCLUDE_DIR
  ${text_view_INCLUDE_DIR}
  REALPATH)
set(text_view_INCLUDE_DIRS
      ${text_view_INCLUDE_DIR})
unset(text_view_INCLUDE_DIR)

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

# Include sub-directories.
add_subdirectory(include)
add_subdirectory(test)
add_subdirectory(examples)

# The 'check' target is used to perform a build and then run tests.  This
# approach is done in lieu of a way to make the CMake generated 'test' target
# depend on 'all'.  These rules also exercise building the example utilities
# directly from the build directory to ensure that a build directory can be
# used to build a downstream consumer without requiring an installation.
add_custom_target(
  check
  COMMAND ${CMAKE_COMMAND}
          --build ${CMAKE_BINARY_DIR}
          --target all
  COMMAND ${CMAKE_CTEST_COMMAND}
  COMMAND ${CMAKE_COMMAND}
          -B${CMAKE_BINARY_DIR}/examples/build
          -H${CMAKE_CURRENT_LIST_DIR}/examples
          -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} --build
          ${CMAKE_BINARY_DIR}/examples/build
          --target all
  COMMAND ${CMAKE_COMMAND}
          -E chdir ${CMAKE_BINARY_DIR}/examples/build
          ${CMAKE_CTEST_COMMAND})

# The 'check-install' target is used to exercise installation and to validate
# that the examples build and run as expected in the installation location.
# This target only works on UNIX and UNIX-like systems and with generated
# 'make' based build systems since it relies on being able to perform an
# installation to an arbitrary directory by passing DESTDIR=/path/to/install
# as a command line option when invoking the generated build system.
add_custom_target(
  check-install
  DEPENDS check
  COMMAND ${CMAKE_COMMAND}
          --build ${CMAKE_BINARY_DIR}
          --target install -- DESTDIR=${CMAKE_BINARY_DIR}/install
  COMMAND ${CMAKE_COMMAND}
          -B${CMAKE_BINARY_DIR}/install/${CMAKE_INSTALL_PREFIX}/${TEXT_VIEW_DESTINATION_SHARE}/examples/build
          -H${CMAKE_BINARY_DIR}/install/${CMAKE_INSTALL_PREFIX}/${TEXT_VIEW_DESTINATION_SHARE}/examples
          -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/${CMAKE_INSTALL_PREFIX}
  COMMAND ${CMAKE_COMMAND} --build
          ${CMAKE_BINARY_DIR}/install/${CMAKE_INSTALL_PREFIX}/${TEXT_VIEW_DESTINATION_SHARE}/examples/build
          --target all
  COMMAND ${CMAKE_COMMAND}
          -E chdir ${CMAKE_BINARY_DIR}/install/${CMAKE_INSTALL_PREFIX}/${TEXT_VIEW_DESTINATION_SHARE}/examples/build
          ${CMAKE_CTEST_COMMAND})

# Package configuration module generation commands.
include(CMakePackageConfigHelpers)
configure_package_config_file(
  cmake/text_view-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/text_view-config.cmake
  INSTALL_DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE})

# Export commands to allow the build tree to be used in lieu of an installation.
export(
  EXPORT text_view-targets
  FILE cmake/text_view-targets.cmake)
file(
  COPY cmake/modules/FindCMCSTL2.cmake
  DESTINATION cmake/modules)

# Install commands.
install(
  FILES LICENSE.txt README.md
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE})
install(
  DIRECTORY include/
  DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE}
  COMPONENT development)
install(
  EXPORT text_view-targets
  FILE text_view-targets.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}
  COMPONENT development)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/text_view-config.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}
  COMPONENT development)
install(
  FILES cmake/modules/FindCMCSTL2.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}/modules
  COMPONENT development)
install(
  DIRECTORY examples/
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE}/examples
  COMPONENT examples)
