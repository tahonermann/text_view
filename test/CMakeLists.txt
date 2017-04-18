# Copyright (c) 2017, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_test CXX)

find_package(text_view REQUIRED)

add_executable(
  test-encodings
  test-encodings.cpp)
target_compile_options(
  test-encodings
  PRIVATE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  test-encodings
  PRIVATE ${text_view_DEFINITIONS})
target_include_directories(
  test-encodings
  PRIVATE ${text_view_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-encodings
  COMMAND test-encodings)

add_executable(
  test-models
  test-models.cpp)
target_compile_options(
  test-models
  PRIVATE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  test-models
  PRIVATE ${text_view_DEFINITIONS})
target_include_directories(
  test-models
  PRIVATE ${text_view_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-models
  COMMAND test-models)

add_executable(
  test-subobject
  test-subobject.cpp)
target_compile_options(
  test-subobject
  PRIVATE ${text_view_COMPILE_OPTIONS})
target_compile_definitions(
  test-subobject
  PRIVATE ${text_view_DEFINITIONS})
target_include_directories(
  test-subobject
  PRIVATE ${text_view_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-subobject
  COMMAND test-subobject)
