CC=gcc
CFLAGS=-Wall

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

repl: repl.o
	$(CC) -o repl -Wall repl.c
