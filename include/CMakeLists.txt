# Copyright (c) 2017, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_include CXX)

add_library(
  text-view
  INTERFACE)
target_compile_options(
  text-view
  INTERFACE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  text-view
  INTERFACE ${text_view_DEFINITIONS})
target_include_directories(
  text-view
  INTERFACE
    $<BUILD_INTERFACE:${text_view_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:${TEXT_VIEW_DESTINATION_INCLUDE}>)
target_link_libraries(
  text-view
  INTERFACE CMCSTL2)

install(
  TARGETS text-view
  EXPORT text_view-targets
  INCLUDES DESTINATION ${TEXT_VIEW_DESTINATION_INCLUDE})
