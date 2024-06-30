all: main

run: main
	./main

main: main.o kosaraju.o
	g++ -o main main.o kosaraju.o -lpthread

main.o: main.cpp kosaraju.hpp
	g++ -c main.cpp

kosaraju.o: kosaraju.cpp kosaraju.hpp
	g++ -c kosaraju.cpp

clean:
	rm -f main main.o kosaraju.o

.PHONY: all run clean