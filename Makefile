.PHONY: all clean

all: shell

shell: shell.c
	cc -o shell shell.c

clean:
	rm -rfv shell

