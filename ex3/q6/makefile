
all: libreactor.a

libreactor.a: reactor.o
	ar rcs libreactor.a reactor.o
	ranlib libreactor.a

reactor.o: reactor.cpp reactor.hpp
	g++ -std=c++2a -c reactor.cpp

clean:
	rm -f libreactor.a reactor.o

.PHONY: all clean