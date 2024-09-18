/*
This code is base on the code from https://www.geeksforgeeks.org/fleurys-algorithm-for-printing-eulerian-path/
*/
#include "EulerCircle.hpp"

#include <string.h>

/**
 * @brief Check if the given graph is Eulerian (i.e., it has an Eulerian circuit).
 *
 * @param graph The graph to check.
 * @return true If the graph is Eulerian.
 */
bool Graph::isEulerian() {
    // Check if each vertex has even degree
    for (size_t i = 0; i < adjMat.size(); i++) {
        // Count the number of edges incident on the vertex
        int degree = 0;
        for (size_t j = 0; j < adjMat.size(); j++) {
            if (adjMat[i][j] != NO_EDGE)
                degree += 1;
        }
        // If the degree is odd, the graph is not Eulerian
        if (degree % 2 != 0) {
            return false;
        }
    }
    return true;
}

void Graph::printEulerCycle() {
    if (isEulerian()) {
        printEulerUtil(0);
    } else {
        cout << "Graph is not Eulerian" << endl;
    }
}

// Print Euler tour starting from vertex u
void Graph::printEulerUtil(int u) {
    int n = adjMat.size();
    // Recur for all the vertices adjacent to this vertex
    for (int v = 0; v != n; ++v) {
        // If edge u-v is not removed and it's a a valid
        // next edge
        if (adjMat[u][v] != NO_EDGE && isValidNextEdge(u, v)) {
            cout << u << "-" << v << " ";
            rmvEdge(u, v);
            printEulerUtil(v);
        }
    }
}

// The function to check if edge u-v can be considered as
// next edge in Euler Tout
bool Graph::isValidNextEdge(int u, int v) {
    int n = adjMat.size();
    // The edge u-v is valid in one of the following two
    // cases:

    // 1) If v is the only adjacent vertex of u
    int count = 0;  // To store count of adjacent vertices

    for (int i = 0; i != n; ++i) {
        if (adjMat[u][i] != NO_EDGE) {
            count++;
        }
    }
    if (count == 1) return true;

    // 2) If there are multiple adjacents, then u-v is not a
    // bridge Do following steps to check if u-v is a bridge

    // 2.a) count of vertices reachable from u
    bool visited[n];
    memset(visited, false, n);
    int count1 = DFSCount(u, visited);

    // 2.b) Remove edge (u, v) and after removing the edge,
    // count vertices reachable from u
    rmvEdge(u, v);
    memset(visited, false, n);
    int count2 = DFSCount(u, visited);

    // 2.c) Add the edge back to the graph
    addEdge(u, v);

    // 2.d) If count1 is greater, then edge (u, v) is a
    // bridge
    return (count1 > count2) ? false : true;
}

// This function removes edge u-v from graph. It removes
// the edge by replacing adjacent vertex value with NO_EDGE.
void Graph::rmvEdge(int u, int v) {
    // Find v in adjacency list of u and replace it with NO_EDGE
    adjMat[u][v] = NO_EDGE;

    // Find u in adjacency list of v and replace it with NO_EDGE
    adjMat[v][u] = NO_EDGE;
}

// A DFS based function to count reachable vertices from v
int Graph::DFSCount(int v, bool visited[]) {
    // Mark the current node as visited
    visited[v] = true;
    int count = 1;

    // Recur for all vertices adjacent to this vertex
    for (size_t i = 0; i != adjMat.size(); ++i) {
        if (adjMat[v][i] != NO_EDGE && !visited[i]) {
            count += DFSCount(i, visited);
        }
    }

    return count;
}
