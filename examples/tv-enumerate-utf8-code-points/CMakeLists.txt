# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

add_executable(
  tv-enumerate-utf8-code-points
  tv-enumerate-utf8-code-points.cpp)

add_test(
  NAME tv-enumerate-utf8-code-points
  COMMAND tv-enumerate-utf8-code-points)
