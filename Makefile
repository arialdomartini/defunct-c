IDIR =../include
CC=gcc
ODIR=obj
CFLAGS=-std=c99 -Wall -I$(IDIR) -ledit -lm

_OBJ = defunct.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

defunct: $(OBJ)
	$(CC) -o $@ mpc.c $^ $(CFLAGS)


.PHONY: clean

clean:
	rm -fr $(ODIR) *~ defunct $(INCDIR)/*~ 
