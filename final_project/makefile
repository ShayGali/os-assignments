CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall

PROG=main

.PHONY: clean all run

all: $(PROG)

run: $(PROG)
	./$(PROG)

clean:
	rm -f $(PROG) *.o utils/*.o

main: main.o Graph.o TreeOnGraph.o MSTFactory.o MSTSolver.o utils/union_find.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp Graph.hpp TreeOnGraph.hpp MSTSolver.hpp utils/union_find.hpp MSTFactory.hpp client_commands.hpp
	$(CXX) $(CXXFLAGS) -c $<

Graph.o: Graph.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c $<

TreeOnGraph.o: TreeOnGraph.cpp TreeOnGraph.hpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c $<

MSTSolver.o: MSTSolver.cpp MSTSolver.hpp Graph.hpp TreeOnGraph.hpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) -c $<

MSTFactory.o: MSTFactory.cpp MSTFactory.hpp MSTSolver.hpp client_commands.hpp
	$(CXX) $(CXXFLAGS) -c $<

utils/union_find.o: utils/union_find.cpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $<