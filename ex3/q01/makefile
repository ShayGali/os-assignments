
all: kosaraju

run: kosaraju
	./kosaraju

kosaraju: kosaraju.cpp
	g++ -std=c++11 -o kosaraju kosaraju.cpp

clean:
	rm -f kosaraju *.o

.PHONY: clean run all