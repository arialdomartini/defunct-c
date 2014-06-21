IDIR =../include
CC=gcc
ODIR=obj
CFLAGS=-Wall -I$(IDIR) -ledit

_OBJ = defunct.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

defunct: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -fr $(ODIR) *~ defunct $(INCDIR)/*~ 
