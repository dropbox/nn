.PHONY: run_test

all: run_test

run_test: test
	./test && echo "test passed"

test: nn.hpp test_nn.cpp
	c++ -std=c++14 test_nn.cpp -o test
