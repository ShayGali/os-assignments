#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
constexpr int NO_EDGE = 0;

// A class that represents an undirected graph
class Graph {
   private:
    vector<vector<int>> adjMat;  // Adjacency matrix

    // helper functions for finding Eulerian cycle
    bool isEulerian();
    void printEulerUtil(int s);

    // This function returns count of vertices reachable
    // from v. It does DFS
    int DFSCount(int v, bool visited[]);

    // Utility function to check if edge u-v is a valid next
    // edge in Eulerian trail or circuit
    bool isValidNextEdge(int u, int v);

   public:
    // Constructor and destructor
    Graph(int n) {
        adjMat = vector<vector<int>>(n, vector<int>(n, NO_EDGE));
    }
    // functions to add and remove edge
    void addEdge(int u, int v) {
        adjMat[u][v] = 1;
        adjMat[v][u] = 1;
    }

    void rmvEdge(int u, int v);

    bool isEdge(int u, int v) {
        return adjMat[u][v] != NO_EDGE;
    }

    // Methods to print Eulerian tour
    void printEulerCycle();
};
