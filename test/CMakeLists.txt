# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(test-text-view CXX)

find_package(text_view REQUIRED)

add_executable(
  test-text-view
  test-text_view.cpp)
target_compile_options(
  test-text-view
  PRIVATE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  test-text-view
  PRIVATE ${text_view_DEFINITIONS})
target_include_directories(
  test-text-view
  PRIVATE ${text_view_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-text-view
  COMMAND test-text-view)
