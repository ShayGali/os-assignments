#pragma once

#include <string>

#include "Graph.hpp"
#include "MST_solver.hpp"

using std::string;

class MST_Factory {
   public:
    static MST_Solver* createMSTSolver(const string& solverType);
};
