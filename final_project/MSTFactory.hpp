#pragma once

#include <string>

#include "Graph.hpp"
#include "MST_solver.hpp"

using std::string;

class MST_Factory {
   public:
    /**
     * @brief Create a MST Solver object.
     * A solver will be dinamically allocated and returned.
     * It is the caller's responsibility to free the memory.
     *
     * @param solverType The type of the solver to be created.
     * @return MST_Solver* A pointer to the created solver.
     */
    static MST_Solver* createMSTSolver(const string& solverType);
};
