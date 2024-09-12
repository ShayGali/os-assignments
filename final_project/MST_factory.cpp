#include "MST_factory.hpp"

#include <stdexcept>

MST_Solver* MST_Factory::createMSTSolver(const string& solverType) {
    if (solverType == "Kruskal") {
        return new Kruskal();
    } else if (solverType == "Prim") {
        return new Prim();
    } else {
        throw std::invalid_argument("Invalid solver type");
    }
}