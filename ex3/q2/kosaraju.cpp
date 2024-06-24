#include <iostream>
#include <vector>
using namespace std;

/**
 * dfs function starting from node
 * @param node - starting node
 * @param adj_matrix - adjacency matrix
 * @param visited - visited nodes
 * @param order - result order
 */
void dfs_adj_mat(int node, vector<vector<int>>& adj_matrix, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (adj_matrix[node][i] == 1 && !visited[i]) {
            dfs_adj_mat(i, adj_matrix, visited, order);
        }
    }
    order.push_back(node);
}

// kosaraju algorithm for finding strongly connected components
vector<vector<int>> kosaraju_adj_mat(vector<vector<int>>& adj_matrix) {
    // first dfs
    vector<bool> visited(adj_matrix.size(), false);
    vector<int> order;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (!visited[i]) {
            dfs_adj_mat(i, adj_matrix, visited, order);
        }
    }

    // transpose matrix
    vector<vector<int>> adj_matrix_transposed(adj_matrix.size(), vector<int>(adj_matrix.size(), 0));
    for (int i = 0; i < adj_matrix.size(); i++) {
        for (int j = 0; j < adj_matrix.size(); j++) {
            adj_matrix_transposed[i][j] = adj_matrix[j][i];
        }
    }

    // second dfs- when we iterate over the order
    vector<vector<int>> components;
    fill(visited.begin(), visited.end(), false);
    for (int i = order.size() - 1; i >= 0; i--) {
        int node = order[i];
        if (!visited[node]) {
            vector<int> component;
            dfs_adj_mat(node, adj_matrix_transposed, visited, component);
            components.push_back(component);
        }
    }
    return components;
}

void dfs_adj_list(int node, vector<vector<int>>& adj_list, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    for (int i = 0; i < adj_list[node].size(); i++) {
        if (!visited[adj_list[node][i]]) {
            dfs_adj_list(adj_list[node][i], adj_list, visited, order);
        }
    }
    order.push_back(node);
}

// kosaraju algorithm for finding strongly connected components
vector<vector<int>> kosaraju_adj_list(vector<vector<int>>& adj_list) {
    // first dfs
    vector<bool> visited(adj_list.size(), false);
    vector<int> order;
    for (int i = 0; i < adj_list.size(); i++) {
        if (!visited[i]) {
            dfs_adj_list(i, adj_list, visited, order);
        }
    }

    // transpose matrix
    vector<vector<int>> adj_list_transposed(adj_list.size());
    for (int i = 0; i < adj_list.size(); i++) {
        for (int j = 0; j < adj_list[i].size(); j++) {
            adj_list_transposed[adj_list[i][j]].push_back(i);
        }
    }

    // second dfs- when we iterate over the order
    vector<vector<int>> components;
    fill(visited.begin(), visited.end(), false);
    for (int i = order.size() - 1; i >= 0; i--) {
        int node = order[i];
        if (!visited[node]) {
            vector<int> component;
            dfs_adj_list(node, adj_list_transposed, visited, component);
            components.push_back(component);
        }
    }
    return components;
}

vector<vector<int>> gen_adj_matrix(int n, int seed = 0) {
    srand(seed);
    // init n x n matrix
    vector<vector<int>> adj_matrix(n, vector<int>(n, 0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j)
                adj_matrix[i][j] = rand() % 2;
        }
    }

    return adj_matrix;
}

vector<vector<int>> gen_adj_list(int n, int seed = 0) {
    srand(seed);
    // init n lists
    vector<vector<int>> adj_list(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                adj_list[i].push_back(j);
            }
        }
    }

    return adj_list;
}

int main() {
    int n = 10000;
    cout << "gen_adj_list\n";
    vector<vector<int>> adj_list = gen_adj_list(n);
    cout << "gen_adj_matrix\n";
    vector<vector<int>> adj_matrix = gen_adj_matrix(n);

    cout << "kosaraju_adj_list\n";
    kosaraju_adj_list(adj_list);

    cout << "kosaraju_adj_mat\n";
    kosaraju_adj_mat(adj_matrix);
}