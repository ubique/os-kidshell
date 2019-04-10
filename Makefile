# Makefile

DIR := bin
CXX := clang++

all: dir main test

dir:
	mkdir -p bin

main:
	$(CXX) main.cpp -o $(DIR)/main

test:
	$(CXX) test.cpp -o $(DIR)/test

clean:
	rm -rf $(DIR)