CCX = g++
CXXFLAGS = -std=c++2a -Wall -g 

all: libactor.a

libactor.a: reactor.o proactor.o
	ar rcs libactor.a reactor.o proactor.o
	ranlib libactor.a

reactor.o: reactor.cpp reactor.hpp
	$(CCX) $(CXXFLAGS) -c reactor.cpp

proactor.o: proactor.cpp proactor.hpp
	$(CCX) $(CXXFLAGS) -c proactor.cpp

clean:
	rm -f *.a *.o main

.PHONY: all clean