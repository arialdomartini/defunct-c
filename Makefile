CC=gcc

repl: repl.c
	$(CC) -o repl -Wall repl.c