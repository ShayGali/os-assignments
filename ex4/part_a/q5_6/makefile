CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: hello

valgrind: hello
	valgrind --leak-check=full -s --track-origins=yes --show-leak-kinds=all  --error-exitcode=99 ./hello

debug: hello
	valgrind --leak-check=full --vgdb=yes --vgdb-error=0 ./hello

hello:
	$(CC) $(CFLAGS) -o hello hello.c

clean:
	rm -f hello *.o

#gdb hello
#(gdb) target remote | vgdb