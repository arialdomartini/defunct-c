IDIR =../include
CC=gcc
ODIR=obj
CFLAGS=-Wall -I$(IDIR)

_OBJ = repl.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

repl: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -fr $(ODIR) *~ repl $(INCDIR)/*~ 