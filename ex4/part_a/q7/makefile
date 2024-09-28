CC = gcc
CFLAGS = -Wall -g -pthread

.PHONY: all clean

all: race

helgrind: race
	valgrind --tool=helgrind ./race

race:
	$(CC) $(CFLAGS) -o race race.c

clean:
	rm -f race *.o
