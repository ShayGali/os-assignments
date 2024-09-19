#include "MSTFactory.hpp"

#include <stdexcept>

#include "client_commands.hpp"

MSTSolver* MSTFactory::createMSTSolver(const string& solverType) {
    if (solverType == MST_KRUSKAL) {
        return new Kruskal();
    } else if (solverType == MST_PRIME) {
        return new Prim();
    } else {
        throw std::invalid_argument("Invalid solver type");
    }
}