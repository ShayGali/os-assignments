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
    /**
     * @brief Construct a new Graph object with n vertices.
     */
    Graph(int n=0);

    /**
     * @brief Construct a new Graph object with an adjacency matrix.
     */
    Graph(vector<vector<int>>& adjMat);

    /**
     * @brief Get the number of vertices in the graph.
     */
    int V() const;

    /**
     * @brief Add an edge to the graph.
     */
    void addEdge(int u, int v, int weight);

    /**
     * @brief Remove an edge from the graph.
     */
    void removeEdge(int u, int v);

    /**
     * @brief Check if there is an edge between u and v.
     */
    bool isEdge(int u, int v) const;

    /**
     * @brief Get the weight of the edge between u and v.
     */
    int getWeight(int u, int v) const;

    /**
     * @brief Get the edges of the graph.
     */
    vector<Edge> getEdges() const;

    /**
     * @brief Get the neighbors of a vertex.
     */
    vector<int> getNeighbors(int u) const;

    /**
     * @brief Get the adjacency matrix of the graph.
     */
    const vector<vector<int>>& getAdjMat() const;
};