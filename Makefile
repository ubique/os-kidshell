.PHONY: all run

all: shell

run: shell
	./shell

shell: shell.cpp
	g++ -fsanitize=address -g3 shell.cpp -o shell
