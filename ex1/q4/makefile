.PHONY: all run clean

CC = gcc

all: main

main: main.c
	$(CC) -fprofile-arcs -ftest-coverage -c -Wall  -o $@.o $<
	$(CC) -o $@ $@.o -lgcov

# for running the program with the good and bad inputs
# we clean the previous gcov files, compile the program, run the program with the bad inputs and the good input
run: main
	./main < inputs/good_input.txt > /dev/null
	for file in ./inputs/bad_input*.txt; do \
		./main < $$file > /dev/null || true; \
	done
	gcov main.c

clean:
	rm -f *.o main *.gcno *.gcda *.gcov