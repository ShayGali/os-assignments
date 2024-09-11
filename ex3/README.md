# Assignment 3

## Introduction
This assignment is about the implementation of the reactor and proactor patterns.

We use c++ for this assignment.

The assignment instructions can be found [here](./assignment%20instructions.pdf).

> Note, all q2 results can be found [here](./q02/README.md).


## How to run

### Open client and server 
In this section, we will explain how to run the code for q10 (it will be similar for q9 & q7).

1. to compile the code, run `make all` (in the `q10` directory).
2. to run the code, run `./main`.

Now a tcp server will be running on port `9034`.

To open a clients, you can use `telnet` or `nc` (netcat).

To use netcat, run `nc localhost 9034`.


### Client commands
Now to clients can send the following commands:
1. `Newgraph n m`: to create a new graph with n vertices and m edges. Will get an input of m pairs of integers (in the range of 1 to n) representing the edges. (you can send all of the command in one line - `Newgraph 2 1 1 2`).
2. `Newedge u v`: to add an edge between u and v.
3. `Removeedge u v`: to remove an edge between u and v.
4. `Kosaraju`: to find the strongly connected components in the graph.

### Example
After opening a client & server, you can send the [following commands](./example%20input.txt) from the client:
```
Newgraph 5 4
1 2
2 3
3 1
3 4
Newedge 4 5
Newedge 1 3
Newedge 1 3
Removeedge 1 3
Kosaraju
```

The expected output for the client will be:
```
Newgraph 5 4
1 2
2 3
3 1
3 4
Got input: Newgraph 5 4
1 2
2 3
3 1
3 4
2 3
3 1
New graph created
Newedge 4 5
Got input: Newedge 4 5
Edge added
Newedge 1 3
Got input: Newedge 1 3
Edge added
Newedge 1 3
Got input: Newedge 1 3
Invalid edge
Removeedge 1 3
Got input: Removeedge 1 3
Edge removed
Kosaraju
Got input: Kosaraju
Component 0: 2 3 1 
Component 1: 4 
Component 2: 5 
```

The server will print the following:
```
selectserver: waiting for connections on port 9034
server: got new connection
mutex locked by client4
Got input: Newgraph 5 4
1 2
2 3
3 1
3 4
2 3
3 1
New graph created
mutex unlocked
 
mutex locked by client4
Got input: Newedge 4 5
Edge added
mutex unlocked
 
mutex locked by client4
Got input: Newedge 1 3
Edge added
mutex unlocked
 
mutex locked by client4
Got input: Newedge 1 3
Invalid edge
mutex unlocked
 
mutex locked by client4
Got input: Removeedge 1 3
Edge removed
mutex unlocked
 
mutex locked by client4
Got input: Kosaraju
Component 0: 2 3 1 
Component 1: 4 
Component 2: 5 
mutex unlocked
 
At Least 50% of the graph belongs to the same scc
```
