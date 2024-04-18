.PHONY: all clean

CC = gcc

all: main libpoisson.so

main: main.c libpoisson.so
	$(CC) -o main main.c -L. -lpoisson -lm

libpoisson.so: poisson.o
	$(CC) -shared -o libpoisson.so poisson.o

poisson.o: poisson.c poisson.h
	$(CC) -c -fPIC poisson.c

clean:
	rm -f main libpoisson.so poisson.o
