CC=gcc

.PHONY: all clean	

PROG = ttt

all: $(PROG)

$(PROG): $(PROG).c
	$(CC) -Wall -o $@ $^

clean:
	rm -f $(PROG) *.o