#pragma once

#include <vector>

#include "Graph.hpp"

using std::pair;
using std::vector;

class TreeOnGraph {
    Graph graph;
    vector<pair<int, int>> edges;
};