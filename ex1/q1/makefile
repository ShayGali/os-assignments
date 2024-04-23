.PHONY: all clean

CC = gcc

PROGS = bad_prog_1 bad_prog_2 bad_prog_3 bad_prog_1_g bad_prog_2_g bad_prog_3_g

all: $(PROGS)


# for cores with debugging information (with -g option)
bad_prog_%_g: bad_prog_%.c
	$(CC) -c -Wall -g -o $@.o $<
	$(CC) -o $@ $@.o


# for cores without debugging information
bad_prog_%: bad_prog_%.c
	$(CC) -c -Wall -o $@.o $<
	$(CC) -o $@ $@.o


clean:
	rm -f *.o $(PROGS) core