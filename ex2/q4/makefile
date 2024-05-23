CC=gcc
CFLAGS=-Wall -g

.PHONY: all clean	

PROGS=mync ttt

all: $(PROGS)

mync: mynetcat.c
	$(CC) $(CFLAGS) -o $@ $^

ttt: ttt.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(PROGS) *.o