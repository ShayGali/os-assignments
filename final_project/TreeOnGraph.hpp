#pragma once

#include <vector>

#include "Graph.hpp"

using std::pair;
using std::vector;

class TreeOnGraph {
    Graph mst;
    vector<vector<int>> allPairs;

   public:
    TreeOnGraph(Graph& T);
    int getWeight();     // Total weight of the MST
    int longestDist();   // Longest distance between two vertices
    int avgDist();       // Average distance between all two vertices
    int shortestDist();  // Shortest distance between two vertices i,j where (i,j) and edge belongs to MST

   private:
    vector<vector<int>> getAllPairs();
};