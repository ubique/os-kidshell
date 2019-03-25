CXX := g++
OPTIONS := -std=c++17 -O3 -Wall -pedantic

all: nsh

nsh: nsh.o command.o
	$(CXX) nsh.o command.o -o nsh

nsh.o:
	$(CXX) $(OPTIONS) -c nsh.cpp

command.o:
	$(CXX) $(OPTIONS) -c command.cpp

clean:
	rm -rf *.o nsh
