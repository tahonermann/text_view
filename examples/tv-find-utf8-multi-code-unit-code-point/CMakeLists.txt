# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

add_executable(
  tv-find-utf8-multi-code-unit-code-point
  tv-find-utf8-multi-code-unit-code-point.cpp)

add_test(
  NAME tv-find-utf8-multi-code-unit-code-point
  COMMAND tv-find-utf8-multi-code-unit-code-point)
