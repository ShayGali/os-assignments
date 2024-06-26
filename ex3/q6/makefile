CCX = g++
CXXFLAGS = -std=c++2a -Wall -g 
LDFLAGS = -lpthread # for using pthread library

all: libactor.a main

main: main.cpp libactor.a kosaraju.o
	$(CCX) $(CXXFLAGS) -o main main.cpp kosaraju.o libactor.a $(LDFLAGS)

libactor.a: reactor.o
	ar rcs libactor.a reactor.o
	ranlib libactor.a

reactor.o: reactor.cpp reactor.hpp
	$(CCX) $(CXXFLAGS) -c reactor.cpp

kosaraju.o: kosaraju.cpp kosaraju.hpp
	$(CCX) $(CXXFLAGS) -c kosaraju.cpp

clean:
	rm -f libactor.a *.o main

.PHONY: all clean