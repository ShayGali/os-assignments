#include "Graph.hpp"

#include <stdexcept>

Graph::Graph(int n) {
    adjMat = vector<vector<int>>(n, vector<int>(n, NO_EDGE));
}

Graph::Graph(vector<vector<int>>& adjMat) {
    this->adjMat = adjMat;
}

void Graph::addEdge(int u, int v, int weight) {
    if (u < 0 || u >= adjMat.size() || v < 0 || v >= adjMat.size()) {
        throw std::invalid_argument("Invalid vertex index");
    }
    adjMat[u][v] = weight;
}

void Graph::removeEdge(int u, int v) {
    if (u < 0 || u >= adjMat.size() || v < 0 || v >= adjMat.size()) {
        throw std::invalid_argument("Invalid vertex index");
    }
    adjMat[u][v] = NO_EDGE;
}

bool Graph::isEdge(int u, int v) const {
    if (u < 0 || u >= adjMat.size() || v < 0 || v >= adjMat.size()) {
        throw std::invalid_argument("Invalid vertex index");
    }
    return adjMat[u][v] != NO_EDGE;
}

vector<int> Graph::getNeighbors(int u) const {
    if (u < 0 || u >= adjMat.size()) {
        throw std::invalid_argument("Invalid vertex index");
    }
    vector<int> neighbors;
    for (int i = 0; i < adjMat.size(); i++) {
        if (adjMat[u][i] != NO_EDGE) {
            neighbors.push_back(i);
        }
    }

    return neighbors;
}

const vector<vector<int>>& Graph::getAdjMat() const {
    return adjMat;
}

int Graph::getWeight(int u, int v) const {
    if (u < 0 || u >= adjMat.size() || v < 0 || v >= adjMat.size()) {
        throw std::invalid_argument("Invalid vertex index");
    }
    return adjMat[u][v];
}

int Graph::V() const {
    return adjMat.size();
}

/**
 * @brief Get the edges of the graph. (the graph is undirected)
 */
vector<Edge> Graph::getEdges() const {
    // the graph is undirected, so we need to iterate only over the upper triangle of the matrix
    vector<Edge> edges;
    for (int i = 0; i < adjMat.size(); i++) {
        for (int j = i + 1; j < adjMat.size(); j++) {
            if (adjMat[i][j] != NO_EDGE) {
                edges.push_back({i, j, adjMat[i][j]});
            }
        }
    }

    return edges;
}

string Graph::toString() const {
    string str = "";
    for (int i = 0; i < adjMat.size(); i++) {
        for (int j = i + 1; j < adjMat.size(); j++) {
            if (adjMat[i][j] != NO_EDGE) {
                str += std::to_string(i + 1) + " <-> " + std::to_string(j + 1) + " , " + std::to_string(adjMat[i][j]) + "\n";
            }
        }
    }
    return str;
}