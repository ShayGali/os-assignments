.PHONY: all clean

CC = gcc

all: Poisson

Poisson: Poisson.c
	$(CC) -c -Wall  -o $@.o $<
	$(CC) -o $@ $@.o -lm

clean:
	rm -f *.o Poisson