#include "TreeOnGraph.hpp"

#include <climits>
#include <queue>
#include <stdexcept>

using std::queue;

TreeOnGraph::TreeOnGraph(Graph& T) : mst(T) {
    allPairs = getAllPairs();

    size_t n = mst.getAdjMat().size();

    // check if the graph is connected
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (!(allPairs[i][j] > 0 || (i == j && allPairs[i][j] == 0))) {
                throw std::runtime_error("vertices: " + std::to_string(i) + " and " + std::to_string(j) + " are not connected");
            }
        }
    }

    // Cheking ot T that we have n-1 edges
    if (mst.getEdges().size() != n - 1) {
        throw std::runtime_error("The graph is not a tree");
    }
}

int TreeOnGraph::getWeight() {
    int weight = 0;
    vector<vector<int>> mat = mst.getAdjMat();
    for (size_t i = 0; i < mat.size(); i++) {
        for (size_t j = i + 1; j < mat.size(); j++) {
            if (mat[i][j] != NO_EDGE) {
                weight += mat[i][j];
            }
        }
    }
    return weight;
}

int TreeOnGraph::longestDist() {
    int longest = 0;
    for (size_t i = 0; i < allPairs.size(); i++) {
        for (size_t j = i + 1; j < allPairs.size(); j++) {
            if (allPairs[i][j] > longest) {
                longest = allPairs[i][j];
            }
        }
    }
    return longest;
}

int TreeOnGraph::avgDist() {
    int sum = 0;
    for (size_t i = 0; i < allPairs.size(); i++) {
        for (size_t j = i; j < allPairs.size(); j++) {
            sum += allPairs[i][j];
        }
    }
    return sum / (allPairs.size() * (allPairs.size() - 1) / 2);
}

int TreeOnGraph::shortestDist() {
    int shortest = INT_MAX;
    vector<vector<int>> mat = mst.getAdjMat();
    for (size_t i = 0; i < mat.size(); i++) {
        for (size_t j = i + 1; j < mat.size(); j++) {
            if (mat[i][j] != NO_EDGE && mat[i][j] < shortest) {
                shortest = mat[i][j];
            }
        }
    }
    return shortest;
}

string TreeOnGraph::toString() {
    string str = "";
    vector<vector<int>> mat = mst.getAdjMat();
    for (size_t i = 0; i < mat.size(); i++) {
        for (size_t j = i + 1; j < mat.size(); j++) {
            if (mat[i][j] != NO_EDGE) {
                str += std::to_string(i + 1) + " <-> " + std::to_string(j + 1) + " , " + std::to_string(mat[i][j]) + "\n";
            }
        }
    }
    return str;
}

vector<vector<int>> TreeOnGraph::getAllPairs() {
    vector<vector<int>> mat = mst.getAdjMat();
    vector<vector<int>> allPairs;
    for (size_t i = 0; i < mat.size(); i++) {
        vector<int> distances = getDistances(i);
        allPairs.push_back(distances);
    }
    for (size_t i = 0; i < allPairs.size(); i++) {
        allPairs[i][i] = 0;
    }
    return allPairs;
}

vector<int> TreeOnGraph::getDistances(int root) {
    vector<vector<int>> mat = mst.getAdjMat();
    vector<int> distances(mat.size(), -1);
    distances[root] = 0;
    queue<int> q;
    q.push(root);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (size_t v = 0; v < mat.size(); v++) {
            if (mat[u][v] != NO_EDGE && distances[v] == NO_EDGE) {
                distances[v] = distances[u] + mat[u][v];
                q.push(v);
            }
        }
    }
    return distances;
}
