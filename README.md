# OS Assignments

## Introduction
This repository contains the assignments of Operating Systems course.

## Assignments

### [Assignment 1](ex1)

In this assignment we start using `c` language and `linux` os in advanced way.

We leran about processes, signals, and how to use them in `c` language.

In addition, we learned about `gcov` & `gprof`, and how to use `man` to read the manual of a command.

#### Authors
* [Shay Gali](https://github.com/ShayGali)
* [Hagay Cohen](https://github.com/hagaycohen2)

### [Assignment 2](ex2)

In this assignment we implement a copy of the `nc` (netcat) program.

We learn about sockets (TCP, UDP, UDS), and how to use them in `c` language.

For full details, see the [assignment](ex2/README.md) description.

#### Authors
* [Shay Gali](https://github.com/ShayGali)
* [Hagay Cohen](https://github.com/hagaycohen2)


### [Assignment 3](ex3)

In this assignment we learn about threads, and the `Reactor` & `Proactor` design patterns.

For full details, see the [assignment](ex3/README.md) description.

#### Authors
* [Shay Gali](https://github.com/ShayGali)
* [Hagay Cohen](https://github.com/hagaycohen2)


### [Assignment 4](ex4)

In this assignment we learn how to use `valgrind` in the right way. we learn about `memcheck`, `helgrind`, `callgrind`.

the assignment had 4 parts:

#### [Part 1](ex4/part_a/q1_4/)
We implement graph data structure, and we found a eulerian cycle in a graph.

We use `valgrind memcheck` to check for memory leaks, and `gprof` `callgrind` to check for performance issues.

#### [Part 2](ex4/part_a/q5_6/)

We got a code that has memory leaks and we need to detect them using `valgrind`, and use `dgb` along with `valgrind` to find the bugs.

#### [Part 3](ex4/part_a/q7/)

We got a code that have a race condition, and we need to detect it using `helgrind`.

#### [Part 4](ex4/part_b/)
We implement two design patterns: 
1. Generic thread safe `Singleton` design pattern.
2. The `Guard` design pattern.


#### Authors
* [Shay Gali](https://github.com/ShayGali)
* [Hagay Cohen](https://github.com/hagaycohen2)
* [Chanan Helman](https://github.com/chanan-hash)

### [Final Project](final_project)

In this project we implement a server that can handle multiple clients at the same time, that use the `Leader Follower` & `Pipeline` design patterns.

For full details, see the [assignment](final_project/README.md) description.

#### Authors
* [Shay Gali](https://github.com/ShayGali)
* [Hagay Cohen](https://github.com/hagaycohen2)
* [Chanan Helman](https://github.com/chanan-hash)