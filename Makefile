#Makefile
Compilator := g++
Flags := -std=c++14
Source := main.cpp
Executable := kidshell

all: $(Executable)

run: $(Executable)
	./$(Executable)

$(Executable):
	$(Compilator) $(Flags) $(Source) -o $(Executable)