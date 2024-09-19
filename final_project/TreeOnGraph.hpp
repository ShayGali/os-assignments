#pragma once

#include <string>
#include <vector>

#include "Graph.hpp"

using std::pair;
using std::string;
using std::vector;

class TreeOnGraph {
    Graph mst;
    vector<vector<int>> allPairs;

   public:
    TreeOnGraph() = default;
    TreeOnGraph(Graph& T);
    int getWeight();     // Total weight of the MST
    int longestDist();   // Longest distance between two vertices
    int avgDist();       // Average distance between all two vertices
    int shortestDist();  // Shortest distance between two vertices i,j where (i,j) and edge belongs to MST
    string toString();   // Returns a string representation of the tree

   private:
    vector<vector<int>> getAllPairs();   // Returns a matrix of distances between all pairs of vertices
    vector<int> getDistances(int root);  // Returns a vector of distances from root to all other vertices
};