CXX := g++
OPTIONS := -std=c++17 -O3 -Wall -pedantic

all: nsh

nsh: nsh.o command.o env.o
	$(CXX) $(OPTIONS) nsh.o command.o env.o -o nsh

nsh.o:
	$(CXX) $(OPTIONS) -c nsh.cpp

command.o:
	$(CXX) $(OPTIONS) -c command.cpp
    
env.o:
	$(CXX) $(OPTIONS) -c env.cpp

clean:
	rm -rf *.o nsh
