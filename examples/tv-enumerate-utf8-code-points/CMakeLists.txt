# Copyright (c) 2017, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view_examples_tv-enumerate-utf8-code-points CXX)

find_package(text_view REQUIRED)

add_executable(
  tv-enumerate-utf8-code-points
  tv-enumerate-utf8-code-points.cpp)
target_link_libraries(
  tv-enumerate-utf8-code-points
  PRIVATE text-view)

include(CTest)
add_test(
  NAME tv-enumerate-utf8-code-points
  COMMAND tv-enumerate-utf8-code-points)
