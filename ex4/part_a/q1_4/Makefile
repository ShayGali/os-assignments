CXX=g++
CXXFLAGS=-std=c++2a -g -Wall

CODE_COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
# PROFILNIG_FLAGS = -pg
VALGRIND_FLAGS=--tool=memcheck -v --leak-check=full --show-leak-kinds=all  --error-exitcode=99
PROG=main

V = 50
E = 100
SEED = 10

.PHONY: clean all run coverage

all: $(PROG)

run: $(PROG)
	./$(PROG) -v $(V) -e $(E) -s $(SEED)

coverage: $(PROG)
	# nurmal run
	./$(PROG) -v $(V) -e $(E) -s $(SEED) > /dev/null
	# graph with defenatly euler circle (K3)
	./$(PROG) -v 3 -e 3 -s 0 > /dev/null
	# run wuth bad parameters
	./$(PROG) -l > /dev/null || true
	./$(PROG) > /dev/null || true
	./$(PROG) -v -15 -e -15 -s -15 > /dev/null || true
 
	# run with to many edges
	./$(PROG) -v 5 -e 100 -s 0 > /dev/null || true

	gcov -r Graph.cpp
	gcov -r $(PROG).cpp


valgrind_mem: $(PROG)
	valgrind $(VALGRIND_FLAGS) ./$(PROG) -v $(V) -e $(E) -s $(SEED) 


valgrind_call: $(PROG)
	valgrind --tool=callgrind ./$(PROG) -v $(V) -e $(E) -s $(SEED)

profiling: $(PROG)
	./$(PROG) -v 251 -e 31375 -s $(SEED) 
	gprof $(PROG) > profiling.txt

clean:
	rm -f $(PROG) *.o

main: main.o Graph.o
	$(CXX) $(CXXFLAGS) $(PROFILNIG_FLAGS) -o $@ $^ -lgcov
	
main.o: main.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c main.cpp -o main.o

Graph.o: Graph.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c Graph.cpp -o Graph.o