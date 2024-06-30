CCX = g++
CXXFLAGS = -std=c++2a -Wall -g 
LDFLAGS = -lpthread # for using pthread library

all: libactor.a main

main: main.cpp libactor.a kosaraju.o
	$(CCX) $(CXXFLAGS) -o main main.cpp kosaraju.o libactor.a $(LDFLAGS)

libactor.a: reactor.o proactor.o
	ar rcs libactor.a reactor.o proactor.o
	ranlib libactor.a

reactor.o: reactor.cpp reactor.hpp
	$(CCX) $(CXXFLAGS) -c reactor.cpp

proactor.o: proactor.cpp proactor.hpp
	$(CCX) $(CXXFLAGS) -c proactor.cpp

kosaraju.o: kosaraju.cpp kosaraju.hpp
	$(CCX) $(CXXFLAGS) -c kosaraju.cpp
clean:
	rm -f *.a *.o main

.PHONY: all clean