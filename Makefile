# Copyright (c) 2015, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

.PHONY: all
all: test examples

.PHONY: test
test: test_test-text_view test_examples

.PHONY: test_test-text_view
test_test-text_view: bin/test-text_view
	./bin/test-text_view

.PHONY: test_examples
test_examples: test_tv_enumerate_utf8_code_points
test_examples: test_tv_find_utf8_multi_code_unit_code_point

.PHONY: test_tv_enumerate_utf8_code_points
test_tv_enumerate_utf8_code_points: bin/tv_enumerate_utf8_code_points
	./bin/tv_enumerate_utf8_code_points

.PHONY: test_tv_find_utf8_multi_code_unit_code_point
test_tv_find_utf8_multi_code_unit_code_point: bin/tv_find_utf8_multi_code_unit_code_point
	./bin/tv_find_utf8_multi_code_unit_code_point

.PHONY: examples
examples: bin/tv_dump
examples: bin/tv_enumerate_utf8_code_points
examples: bin/tv_find_utf8_multi_code_unit_code_point

-include test/test-text_view.d
-include examples/tv_dump.d
-include examples/tv_enumerate_utf8_code_points.d
-include examples/tv_find_utf8_multi_code_unit_code_point.d

bin:
	mkdir bin

bin/test-text_view: test/test-text_view.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF test/test-text_view.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

bin/tv_dump: examples/tv_dump.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF examples/tv_dump.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

bin/tv_enumerate_utf8_code_points: examples/tv_enumerate_utf8_code_points.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF examples/tv_enumerate_utf8_code_points.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

bin/tv_find_utf8_multi_code_unit_code_point: examples/tv_find_utf8_multi_code_unit_code_point.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF examples/tv_find_utf8_multi_code_unit_code_point.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

clean: clean-test
clean: clean-examples
clean-test:
	rm -f bin/test-text_view
	rm -f test/test-text_view.d
clean-examples:
	rm -f bin/tv_dump
	rm -f bin/tv_enumerate_utf8_code_points
	rm -f bin/tv_find_utf8_multi_code_unit_code_point
	rm -f examples/tv_dump.d
	rm -f examples/tv_enumerate_utf8_code_points.d
	rm -f examples/tv_find_utf8_multi_code_unit_code_point.d
