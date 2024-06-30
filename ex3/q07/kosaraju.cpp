#include "kosaraju.hpp"

#include <stdexcept>

/**
 * dfs function starting from node
 * @param node - starting node
 * @param adj_matrix - adjacency matrix
 * @param visited - visited nodes
 * @param order - result order
 */
void dfs(int node, vector<vector<int>>& adj_list, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    for (int i = 0; i < adj_list[node].size(); i++) {
        if (!visited[adj_list[node][i]]) {
            dfs(adj_list[node][i], adj_list, visited, order);
        }
    }
    order.push_back(node);
}

// kosaraju algorithm for finding strongly connected components
vector<vector<int>> kosaraju(vector<vector<int>>& adj_list) {
    // first dfs
    vector<bool> visited(adj_list.size(), false);
    vector<int> order;
    for (int i = 0; i < adj_list.size(); i++) {
        if (!visited[i]) {
            dfs(i, adj_list, visited, order);
        }
    }

    // transpose matrix
    vector<vector<int>> g_transposed(adj_list.size());
    for (int i = 0; i < adj_list.size(); i++) {
        for (int j = 0; j < adj_list[i].size(); j++) {
            g_transposed[adj_list[i][j]].push_back(i);
        }
    }

    // second dfs- when we iterate over the order
    vector<vector<int>> components;
    fill(visited.begin(), visited.end(), false);
    for (int i = order.size() - 1; i >= 0; i--) {
        int node = order[i];
        if (!visited[node]) {
            vector<int> component;
            dfs(node, g_transposed, visited, component);
            components.push_back(component);
        }
    }
    return components;
}

bool remove_edge(vector<vector<int>>& g, int i, int j) {
    // normalize i and j
    i -= 1;
    j -= 1;
    if (i < 0 || i >= g.size() || j < 0 || j >= g.size()) {
        return false;
    }
    // check if i-->j
    for (int k = 0; k < g[i].size(); k++) {
        if (g[i][k] == j) {
            g[i].erase(g[i].begin() + k);
            return true;
        }
    }

    return false;
}

bool add_edge(vector<vector<int>>& g, int i, int j) {
    if (i <= 0 || i > g.size() || j <= 0 || j > g.size()) {
        return false;
    }

    // normalize i and j
    i--;
    j--;

    // check if i-->j
    for (int k = 0; k < g[i].size(); k++) {
        if (g[i][k] == j) {
            return false;
        }
    }

    g[i].push_back(j);

    return true;
}
