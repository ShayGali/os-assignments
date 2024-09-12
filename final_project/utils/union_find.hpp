// https://github.com/FranciscoThiesen/karger-klein-tarjan/blob/master/union_find/union_find.hpp

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>

// Implementation of Union Find (Disjoint Set Union)
// Code includes Path Compression and Union by Rank for speeding it up
// Complexity: unite -> O( inverse_ack(n) ), find_parent( inverse_ack(n) )
struct UnionFind {
    /**
     * @brief Construct a new Union Find object
     */
    UnionFind(int _n);

    /**
     * @brief Find the parent of a node
     *
     * @param node Node to find the parent
     * @return int Parent of the node
     */
    int find_parent(int node);

    /**
     * @brief Unite two nodes. if the two nodes are already united, return false
     *
     * @param x Node 1
     * @param y Node 2
     *
     * @return true if the nodes were united, false otherwise
     */
    bool unite(int x, int y);
    std::vector<int> parent, rank;
    int n, cc;
};

#endif