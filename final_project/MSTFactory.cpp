#include <stdexcept>

#include "MSTFactory.hpp"
#include "client_commands.hpp"

MST_Solver* MST_Factory::createMSTSolver(const string& solverType) {
    if (solverType == MST_KRUSKAL) {
        return new Kruskal();
    } else if (solverType == MST_PRIME) {
        return new Prim();
    } else {
        throw std::invalid_argument("Invalid solver type");
    }
}