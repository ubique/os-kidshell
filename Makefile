CC = gcc
CFLAGS = -std=c99 -ansi
SRCS = src/main.c src/exit_messages.c
OBJS = $(SRCS: .c = .o)
PROG = interpreter

all: $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) -o $(PROG)

run: all
	./$(PROG)

clean:
	rm *.o

test: all
	clear
	test/test.sh

