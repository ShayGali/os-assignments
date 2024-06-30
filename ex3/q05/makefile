
all: libactor.a

libactor.a: reactor.o
	ar rcs libactor.a reactor.o
	ranlib libactor.a

reactor.o: reactor.cpp reactor.hpp
	g++ -std=c++2a -c reactor.cpp

clean:
	rm -f libactor.a reactor.o

.PHONY: all clean