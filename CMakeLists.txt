# Copyright (c) 2016, Tristan Brindle, Tom Honermann
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

# Include sub-directories.
add_subdirectory(test)
add_subdirectory(examples)

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

# The 'check' target is used to perform a build and then run tests.  This
# approach is done in lieu of a way to make the CMake generated 'test' target
# depend on 'all'.
add_custom_target(
  check
  COMMAND ${CMAKE_COMMAND}
          --build ${CMAKE_BINARY_DIR}
          --target all
  COMMAND ${CMAKE_CTEST_COMMAND})

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

# Install commands.
install(
  FILES LICENSE.txt README.md
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE})
install(
  DIRECTORY include/
  DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE}
  COMPONENT development)
install(
  DIRECTORY examples/
  DESTINATION ${TEXT_VIEW_DESTINATION_SHARE}/examples
  COMPONENT examples)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/text_view-config.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}
  COMPONENT development)
install(
  FILES cmake/modules/FindCMCSTL2.cmake
  DESTINATION ${TEXT_VIEW_DESTINATION_CMAKE}/modules
  COMPONENT development)
