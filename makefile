all: shell examples

shell:
	g++ shell.cpp -o shell

examples:
	g++ echo.cpp -o echo
	g++ hello.cpp -o hello

run: all
	./shell
	
clean:
	rm -f shell hello echo
