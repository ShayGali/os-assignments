CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall

PROG=main

.PHONY: clean all run

all: $(PROG)

run: $(PROG)
	./$(PROG)

clean:
	rm -f $(PROG) *.o

main: main.o Graph.o TreeOnGraph.o MST_factory.o MST_solver.o utils/union_find.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp Graph.hpp TreeOnGraph.hpp MST_solver.hpp utils/union_find.hpp MST_factory.hpp client_commands.hpp
	$(CXX) $(CXXFLAGS) -c $<

Graph.o: Graph.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c $<

TreeOnGraph.o: TreeOnGraph.cpp TreeOnGraph.hpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c $<

MST_solver.o: MST_solver.cpp MST_solver.hpp Graph.hpp TreeOnGraph.hpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) -c $<

MST_factory.o: MST_factory.cpp MST_factory.hpp MST_solver.hpp
	$(CXX) $(CXXFLAGS) -c $<

utils/union_find.o: utils/union_find.cpp utils/union_find.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $<