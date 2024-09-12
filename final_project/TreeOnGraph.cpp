#include "TreeOnGraph.hpp"

#include <queue>
using std::queue;

TreeOnGraph::TreeOnGraph(Graph& T) : mst(T) {
    allPairs = getAllPairs();
}

int TreeOnGraph::getWeight() {}
int TreeOnGraph::longestDist() {}
int TreeOnGraph::avgDist() {}
int TreeOnGraph::shortestDist() {}

vector<vector<int>> TreeOnGraph::getAllPairs() {
    vector<vector<int>> mat = mst.getAdjMat();
    int n = mat.size();
    vector<vector<int>> dist = vector<vector<int>>(n, vector<int>(n, NO_EDGE));
    queue<int> bfs = {};

    for (int i = 1; i < n; i++) {
        if (mat[0][i] != NO_EDGE) {
            dist[i][0] = dist[0][i] = mat[i][0];
            bfs.push(i);
        }
    }
    while (!bfs.empty()) {
        int current = bfs.front();
        bfs.pop();
        for (int i = 1; i < n; i++) {
            if (mat[current][i] != NO_EDGE && dist[current][i] == NO_EDGE) {
                dist[i][0] = dist[0][i] = mat[current][i] + dist[current][0];
                bfs.push(i);
            }
        }
    }

    dist[0][0] = 0;

    for (int i = 1; i < n; i++) {
        for (int j = i; j < n; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else if (mat[i][j] != NO_EDGE) {
                dist[i][j] = mat[i][j];
            } else {
                queue<int> path = {};
                vector<int> hood = mst.getNeighbors(i);
                for
                    k in hood {
                        if (dist[0][k] < dist[i][k}) {
                            /* code */
                        }
                    }

                do {
                } while (path.back != 0);
            }
        }
    }

    return dist;
}