#pragma once

#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<int, int, int> Edge;

using std::string;
using std::vector;

constexpr int NO_EDGE = -1;

class Graph {
   private:
    vector<vector<int>> adjMat;

   public:
    /**
     * @brief Construct a new Graph object with n vertices.
     */
    Graph(int n = 0);

    /**
     * @brief Get the number of vertices in the graph.
     */
    int V() const;

    /**
     * @brief Add an edge to the graph.
     */
    void addEdge(size_t u, size_t v, int weight);

    /**
     * @brief Remove an edge from the graph.
     */
    void removeEdge(size_t u, size_t v);

    /**
     * @brief Get the weight of the edge between u and v.
     */
    int getWeight(size_t u, size_t v) const;

    /**
     * @brief Get the edges of the graph.
     */
    vector<Edge> getEdges() const;

    /**
     * @brief Get the neighbors of a vertex.
     */
    vector<int> getNeighbors(size_t u) const;

    /**
     * @brief Get the adjacency matrix of the graph.
     */
    const vector<vector<int>>& getAdjMat() const;

    string toString() const;
};