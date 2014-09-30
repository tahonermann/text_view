all: test

-include test.d

test: test.cpp
	g++ -g -MMD -std=c++1y test.cpp -Iinclude -I$(ORIGIN_INSTALL_PATH) -o $@

clean:
	rm -f test test.d
