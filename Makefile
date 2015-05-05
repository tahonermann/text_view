all: test

.PHONY: test
test: bin/test-text_view
	./bin/test-text_view

-include test/test-text_view.d

bin:
	mkdir bin

bin/test-text_view: test/test-text_view.cpp | bin
	g++ -Wall -Wpedantic -g -MMD -MF test/test-text_view.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

clean:
	rm -f bin/test-text_view
	rm -f test/test-text_view.d
