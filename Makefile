# Copyright (c) 2015, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

all: test examples

.PHONY: test
test: bin/test-text_view
	./bin/test-text_view

.PHONY: examples
examples: bin/tv_dump

-include test/test-text_view.d
-include examples/tv_dump.d

bin:
	mkdir bin

bin/tv_dump: examples/tv_dump.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF examples/tv_dump.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

bin/test-text_view: test/test-text_view.cpp | bin
	g++ -Wall -Werror -Wpedantic -g -MMD -MF test/test-text_view.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

clean: clean-test
clean: clean-examples
clean-test:
	rm -f bin/test-text_view
	rm -f test/test-text_view.d
clean-examples:
	rm -f bin/tv_dump
	rm -f examples/tv_dump.d
