#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "EulerCircle.hpp"

/**
 * @brief Generate a random undirected graph with the given number of vertices and edges.
 *
 * @param v The number of vertices in the graph.
 * @param e The number of edges in the graph.
 * @param seed The seed for the random number generator.
 *
 * @return The generated graph.
 */
Graph generateRandomGraph(int n, int e, int seed) {
    Graph g(n);

    srand(seed);

    int count = 0;
    while (count < e) {
        int u = rand() % n;
        int v = rand() % n;
        if (u != v && g.isEdge(u, v) == false) {
            g.addEdge(u, v);
            count++;
        }
    }

    return g;
}

// Driver program to test above function
int main(int argc, char *argv[]) {
    int opt;

    int v = -1, e = -1, seed = -1;

    // parse the arguments
    while ((opt = getopt(argc, argv, "v:e:s:")) != -1) {
        switch (opt) {
            case 'v':
                v = std::atoi(optarg);
                break;
            case 'e':
                e = std::atoi(optarg);
                break;
            case 's':
                seed = std::atoi(optarg);
                break;
            default: /* '?' */
                std::cerr << "Usage: " << argv[0] << " -v num_vertices -e num_edges -s seed\n";
                exit(EXIT_FAILURE);
        }
    }

    if (v == -1 || e == -1 || seed == -1) {
        std::cerr << "Usage: " << argv[0] << " -v num_vertices -e num_edges -s seed\n";
        exit(EXIT_FAILURE);
    }

    if (v < 0 || e < 0 || seed < 0) {
        std::cerr << "All arguments must be positive integers\n";
        exit(EXIT_FAILURE);
    }

    // check if we can have `e` edges in a graph with `v` vertices
    if (e > (v * (v - 1)) / 2) {
        std::cerr << "The number of edges cannot be more than " << (v * (v - 1)) / 2 << " for " << v << " vertices\n";
        exit(EXIT_FAILURE);
    }

    Graph g = generateRandomGraph(v, e, seed);

    g.printEulerCycle();
    cout << endl;
    return 0;
}