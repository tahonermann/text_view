# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

add_executable(test-text-view test-text_view.cpp)

add_test(
  NAME test-text-view
  COMMAND test-text-view)
