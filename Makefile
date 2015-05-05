all: test

.PHONY: test
test: bin/test-stdtext
	./bin/test-stdtext

-include test/test-stdtext.d

bin:
	mkdir bin

bin/test-stdtext: test/test-stdtext.cpp | bin
	g++ -Wall -Wpedantic -g -MMD -MF test/test-stdtext.d -std=c++1z $< -Iinclude -I$(ORIGIN_INSTALL_PATH) -I$(BOOST_INSTALL_PATH)/include -o $@

clean:
	rm -f bin/test-stdtext
	rm -f test/test-stdtext.d
