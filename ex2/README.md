# Assignment 2 - netcat implementation

in this assignment we will implement basic implementation of netcat.

we will support the following features:
1. -e flag: execute a shell command
2. -i flag: get input from a socket
3. -o flag: write output to a socket
4. -b flag: input and output go to the same socket
5. -t flag: set timeout for the execution of the program

if we don't send the `-e` flag, the program will act as a chat between two terminals. (if we send `-i` or `-o` flags, the chat will be one-way channel)

we will support the following socket types:
1. TCP - as client & server. can get input, output or both from a socket.
2. UDP - as client & server. can get input, output or both from a socket.
3. UNIX Domain Socket Stream - as client & server. can get input, output or both from a socket.
4. UNIX Domain Socket Datagram - as client & server. server can get input, client will send output.

to use the socket we will send them after the flag, in this format:
1. TCP server: `TCPS<PORT>`
2. TCP client: `TCPC<HOST>,<PORT>`
3. UDP server: `UDPS<PORT>`
4. UDP client: `UDPC<HOST>,<PORT>`
5. UDS stream server: `UDSSS<PATH>`
6. UDS stream client: `UDSCS<PATH>`
7. UDS datagram server: `UDSSD<PATH>` (go only with -i flag)
8. UDS datagram client: `UDSCD<PATH>` (go only with -o flag)


## example usage:
in all of the examples below, when we use the -e flag, we will execute the command a tik tak toe game, that we implemented in the [q1](./q1/ttt.c) folder.

we simulate a server & client with `nc` command,and for the unix domain socket datagram we use `socat` command.

all of the commands that you can use are in the [run_commands](./run_commands) file.


1. open a TCP server and wait for a input from a client. output go to stdout
```bash
# server
./mync -e "./ttt 123456789" -i TCPS4269
# client
nc localhost 4269
```

2. open a TCP server and wait for a input from a client. output go to client
``` bash
# server
./mync -e "./ttt 123456789" -b TCPS4269
# client
nc localhost 4269
```

3. open a UDP server and wait for a input from a client, the output go to TCP server that listen on port 6699
```bash
# TCP server
nc -l -p 6699
# UDP server & TCP client
./mync -e "./ttt 123456789" -i UDPS4269 -o TCPClocalhost,6699
# UDP client
nc -u localhost 4269
```

4. open a UDS stream server and wait for a input from a client, the output go to TCP server that listen on port 6699
```bash
# TCP server
nc -l -p 6699
# UDS stream server & TCP client
./mync -e "./ttt 123456789" -i UDSSShoi.socket -o TCPClocalhost,6699
# UDS stream client
nc -U hoi.socket
```

5. open a UDS datagram server & UDP server, the input is from the UDS client and the output go to the UDP client
```bash
# servers
./mync -e "./ttt 123456789" -i UDSSDhoi.socket -o UDPS4269
# UDS client
socat - UNIX-SENDTO:hoi.socket
# UDP client
nc -u localhost 4269
```

6. chat between two terminals
```bash
# terminal 1
./mync -b TCPS4269
# terminal 2
./mync -b TCPClocalhost,4269
```

## code coverage
we checked the code coverage of the code using gcov. most of the code is covered, except errors that are related to the socket library.

[link to the code coverage report:](./q6/code%20covrage/mynetcat.c.gcov)
![alt text](q6/code%20covrage/cose%20cov.png)