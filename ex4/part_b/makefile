CXX=clang++
CXXFLAGS=-std=c++2a -g -Wall

PROG=main

.PHONY: clean all run

all: $(PROG)

run: $(PROG)
	./$(PROG)

clean:
	rm -f $(PROG) *.o

main: main.o Singleton.hpp Guard.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<


%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $<