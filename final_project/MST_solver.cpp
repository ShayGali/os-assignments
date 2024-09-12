
#include "MST_Solver.hpp"

#include <algorithm>
#include <climits>
#include <functional>
#include <queue>
#include <tuple>

#include "TreeOnGraph.hpp"
#include "utils/union_find.hpp"

using std::sort;
using std::tuple;
typedef tuple<int, int, int> Edge;

/**
 * @brief Get the MST object with Kruskal's algorithm.
 * Run in O(E log E) time.
 */
TreeOnGraph Kruskal::getMST(Graph &graph) const {
    UnionFind uf(graph.V());
    // vector<pair<int, int>> mst_edges;
    Graph mst(graph.V());

    // add the edges to a vector of tuples (src, dest, weight)
    const vector<Edge> edges = graph.getEdges();

    // sort the edges by weight
    sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
        return std::get<2>(a) < std::get<2>(b);
    });

    for (Edge e : edges) {
        int u = std::get<0>(e);
        int v = std::get<1>(e);
        if (uf.unite(u, v)) {  // if the vertices are in different components
            mst.addEdge(u, v, graph.getWeight(u, v));
        }
    }

    return TreeOnGraph(mst);
}

TreeOnGraph Prim::getMST(Graph &graph) const {
    vector<tuple<int, int, int>> v_k_p;
    for (int i = 0; i < graph.V(); i++) {
        v_k_p.push_back({i, INT_MAX, -1});
    }

    v_k_p[0] = {0, 0, -1};

    // priority queue to get the minimum edge (compar by the key)
    auto cmp = [](tuple<int, int, int> *a, tuple<int, int, int> *b) { return std::get<1>(*a) > std::get<1>(*b); };
    std::priority_queue<tuple<int, int, int> *, vector<tuple<int, int, int> *>, decltype(cmp)> pq(cmp);

    // fill the priority queue with all the vertices
    for (int i = 0; i < graph.V(); i++) {
        pq.push(&v_k_p[i]);
    }

    // the main part of the algorithm
    while (!pq.empty()) {
        tuple<int, int, int> *u = pq.top();
        pq.pop();
        
        for (int v : graph.getNeighbors(std::get<0>(*u))) {
            // copy the pq to a vector
            vector<tuple<int, int, int> *> pq_copy;
            while (!pq.empty()) {
                pq_copy.push_back(pq.top());
                pq.pop();
            }
            // return the elements to the pq
            for (tuple<int, int, int> *t : pq_copy) {
                pq.push(t);
            }

            // check if v is in the pq
            auto it = std::find_if(pq_copy.begin(), pq_copy.end(), [v](tuple<int, int, int> *t) { return std::get<0>(*t) == v; });
            if (it != pq_copy.end()) {
                int w = graph.getWeight(std::get<0>(*u), v);
                if (w < std::get<1>(**it)) {
                    std::get<1>(**it) = w;
                    std::get<2>(**it) = std::get<0>(*u);
                }
            }
        }
    }

    // create the MST
    Graph mst(graph.V());
    for (tuple<int, int, int> t : v_k_p) {
        if (std::get<2>(t) != -1) {
            mst.addEdge(std::get<0>(t), std::get<2>(t), std::get<1>(t));
        }
    }

    return TreeOnGraph(mst);
}