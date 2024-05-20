CC=gcc
CFLAGS=-Wall -g

.PHONY: all clean	

PROGS=mync ttt tcp_server tcp_client

all: $(PROGS)

mync: mynetcat.c
	$(CC) $(CFLAGS) -o $@ $^

ttt: ttt.c
	$(CC) $(CFLAGS) -o $@ $^

tcp_server: tcp_server.c
	$(CC) $(CFLAGS) -o $@ $^

tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(PROGS) *.o