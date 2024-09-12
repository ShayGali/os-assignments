#pragma once

#include <tuple>
#include <vector>

typedef std::tuple<int, int, int> Edge;

using std::vector;

constexpr int NO_EDGE = -1;

class Graph {
   private:
    vector<vector<int>> adjMat;

   public:
    Graph(int n);
    Graph(vector<vector<int>>& adjMat);
    int V() const;
    void addEdge(int u, int v, int weight);
    void removeEdge(int u, int v);
    bool isEdge(int u, int v) const;
    int getWeight(int u, int v) const;
    vector<Edge> getEdges() const;
    vector<int> getNeighbors(int u) const;
    const vector<vector<int>>& getAdjMat() const;
};