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
