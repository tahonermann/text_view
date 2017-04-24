# Copyright (c) 2017, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_src CXX)

add_library(
  text-view
  error_status.cpp)
target_compile_options(
  text-view
  PUBLIC ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  text-view
  PUBLIC ${text_view_DEFINITIONS})
target_include_directories(
  text-view
  PUBLIC
    $<BUILD_INTERFACE:${text_view_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:${TEXT_VIEW_DESTINATION_INCLUDE}>)
target_link_libraries(
  text-view
  PUBLIC CMCSTL2)

install(
  TARGETS text-view
  EXPORT text_view-targets
  RUNTIME DESTINATION ${TEXT_VIEW_DESTINATION_LIB}
  LIBRARY DESTINATION ${TEXT_VIEW_DESTINATION_LIB}
  ARCHIVE DESTINATION ${TEXT_VIEW_DESTINATION_LIB}
  INCLUDES DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE})
