.PHONY: all clean

CC = gcc

all: main

main: main.c
	$(CC) -fprofile-arcs -ftest-coverage -c -Wall  -o $@.o $<
	$(CC) -o $@ $@.o -lgcov

clean:
	rm -f *.o main *.gcno *.gcda *.gcov