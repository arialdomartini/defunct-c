CC=gcc
CFLAGS=-Wall -I.
OBJ = repl.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

repl: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

