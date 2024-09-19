#pragma once

#include <string>

#include "Graph.hpp"
#include "MSTSolver.hpp"

using std::string;

class MST_Factory {
   public:
    /**
     * @brief Create a MST Solver object.
     * A solver will be dinamically allocated and returned.
     * It is the caller's responsibility to free the memory.
     *
     * @param solverType The type of the solver to be created.
     * @return MSTSolver* A pointer to the created solver.
     */
    static MSTSolver* createMSTSolver(const string& solverType);
};
