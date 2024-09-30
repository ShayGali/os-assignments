# Default flags (if any)
CODE_COVERAGE_FLAGS = 
# CODE_COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

CXX=g++
CXXFLAGS=-std=c++2a -g -Wall

PROG=main

.PHONY: clean all run coverage valgrind

all: $(PROG)

run: $(PROG)
	./$(PROG)

clean:
	rm -f $(PROG) *.o utils/*.o *.gcno *.gcda *.gcov

coverage: clean
	make CODE_COVERAGE_FLAGS="-fprofile-arcs -ftest-coverage"
	./coverage.sh
	gcov -r main.cpp Graph.cpp TreeOnGraph.cpp MSTSolver.cpp MSTFactory.cpp LFHandler.cpp PipelineHandler.cpp
	mv *.gcov coverage_res/

main: main.o Graph.o TreeOnGraph.o MSTFactory.o MSTSolver.o utils/union_find.o utils/MinHeap.o LFHandler.o PipelineHandler.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -pthread -lgcov

main.o: main.cpp Graph.hpp TreeOnGraph.hpp MSTSolver.hpp utils/union_find.hpp MSTFactory.hpp client_commands.hpp CommandHandler.hpp LFHandler.hpp PipelineHandler.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

Graph.o: Graph.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

TreeOnGraph.o: TreeOnGraph.cpp TreeOnGraph.hpp Graph.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

MSTSolver.o: MSTSolver.cpp MSTSolver.hpp Graph.hpp TreeOnGraph.hpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

MSTFactory.o: MSTFactory.cpp MSTFactory.hpp MSTSolver.hpp client_commands.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

utils/union_find.o: utils/union_find.cpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

utils/MinHeap.o: utils/MinHeap.cpp utils/MinHeap.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

LFHandler.o: LFHandler.cpp LFHandler.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

PipelineHandler.o: PipelineHandler.cpp PipelineHandler.hpp
	$(CXX) $(CXXFLAGS) $(CODE_COVERAGE_FLAGS) -c $<

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $<
