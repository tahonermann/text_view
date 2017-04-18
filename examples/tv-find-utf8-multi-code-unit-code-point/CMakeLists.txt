# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_examples_tv-find-utf8-multi-code-unit-code-point CXX)

find_package(text_view REQUIRED)

add_executable(
  tv-find-utf8-multi-code-unit-code-point
  tv-find-utf8-multi-code-unit-code-point.cpp)
target_compile_options(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_DEFINITIONS})
target_include_directories(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME tv-find-utf8-multi-code-unit-code-point
  COMMAND tv-find-utf8-multi-code-unit-code-point)
