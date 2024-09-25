
#include "MSTSolver.hpp"

#include <algorithm>
#include <climits>
#include <functional>
#include <queue>
#include <tuple>

#include "TreeOnGraph.hpp"
#include "utils/union_find.hpp"
#include "utils/MinHeap.hpp"

using std::sort;
using std::tuple;
using std::vector;

/**
 * @brief Get the MST object with Kruskal's algorithm.
 * Run in O(E log E) time.
 */
TreeOnGraph Kruskal::getMST(Graph &graph) const {
    if (graph.V() == 0) {
        return TreeOnGraph(graph);
    }

    UnionFind uf(graph.V());
    // vector<pair<int, int>> mst_edges;
    Graph mst(graph.V());

    // add the edges to a vector of tuples (src, dest, weight)
    vector<Edge> edges = graph.getEdges();

    // sort the edges by weight
    sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
        return std::get<2>(a) < std::get<2>(b);
    });

    // iterate over the edges, and unoin the vertices if they are not in the same component
    for (Edge e : edges) {
        int u = std::get<0>(e);
        int v = std::get<1>(e);
        if (uf.unite(u, v)) {  // if the vertices are in different components
            mst.addEdge(u, v, graph.getWeight(u, v));
            mst.addEdge(v, u, graph.getWeight(u, v));
        }
    }

    return TreeOnGraph(mst);
}




TreeOnGraph Prim::getMST(Graph &graph) const {
    if (graph.V() == 0) {
        return TreeOnGraph(graph);
    }

    int V = graph.V(); 
    vector<int> parent(V, -1);
    vector<int> key(V, INT_MAX);
    MinHeap minHeap(V);

    key[0] = 0;
    minHeap.insertKey(0, key[0]);

    for (int v = 1; v < V; ++v) {
        minHeap.insertKey(v, key[v]);
    }

    while (!minHeap.isEmpty()) {
        int u = minHeap.extractMin();

        for (int v : graph.getNeighbors(u)) {
            int weight = graph.getWeight(u, v);

            if (minHeap.isInMinHeap(v) && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                minHeap.decreaseKey(v, key[v]);
            }
        }
    }

    Graph mst(V);
    for (int i = 1; i < V; ++i) {
        mst.addEdge(parent[i], i, key[i]);
        mst.addEdge(i, parent[i], key[i]);
    }

    return TreeOnGraph(mst);
}