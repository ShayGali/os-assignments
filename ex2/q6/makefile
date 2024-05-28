CC=gcc
CFLAGS=-Wall -g

.PHONY: all clean	

PROGS=mync ttt

all: $(PROGS)

mync: mynetcat.c
	$(CC) -fprofile-arcs -ftest-coverage $(CFLAGS) -o $@ $^

ttt: ttt.c
	$(CC) -fprofile-arcs -ftest-coverage $(CFLAGS) -o $@ $^

clean:
	rm -f $(PROGS) *.o *.gcno *.gcda *.gcovs