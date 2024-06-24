
all: kosaraju

run: kosaraju
	./kosaraju < input.txt
	gprof kosaraju > output.txt

kosaraju: kosaraju.cpp
	g++ -std=c++11 -pg -o kosaraju kosaraju.cpp

clean:
	rm -f kosaraju

.PHONY: clean run all