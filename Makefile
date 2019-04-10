.PHONY: all clean

all: kidshell

shell: kidshell.h kidshell.c
	cc -o $@ $^

clean:
	rm -rfv kidshell

