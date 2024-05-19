CC=gcc
CFALGS=-Wall -g

.PHONY: all clean	

PROG = mync

all: $(PROG)

$(PROG): $(PROG).c
	$(CC) $(CFALGS) -o $@ $^

clean:
	rm -f $(PROG) *.o