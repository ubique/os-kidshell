all: nsh

nsh: nsh.o command.o
	g++ -std=c++17 nsh.o command.o -o nsh

nsh.o:
	g++ -std=c++17 -c nsh.cpp

command.o:
	g++ -std=c++17 -c command.cpp

clean:
	rm -rf *.o nsh
