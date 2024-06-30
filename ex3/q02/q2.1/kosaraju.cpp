#include <deque>
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
void dfs_deque(int node, vector<vector<int>>& adj_matrix, vector<bool>& visited, deque<int>& order) {
    visited[node] = true;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (adj_matrix[node][i] == 1 && !visited[i]) {
            dfs_deque(i, adj_matrix, visited, order);
        }
    }
    order.push_back(node);
}

// kosaraju algorithm for finding strongly connected components
vector<deque<int>> kosaraju_deque(vector<vector<int>>& adj_matrix) {
    // first dfs
    vector<bool> visited(adj_matrix.size(), false);
    deque<int> order;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (!visited[i]) {
            dfs_deque(i, adj_matrix, visited, order);
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
    vector<deque<int>> components;
    fill(visited.begin(), visited.end(), false);
    for (int i = order.size() - 1; i >= 0; i--) {
        int node = order[i];
        if (!visited[node]) {
            deque<int> component;
            dfs_deque(node, adj_matrix_transposed, visited, component);
            components.push_back(component);
        }
    }
    return components;
}

void dfs_vector(int node, vector<vector<int>>& adj_matrix, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (adj_matrix[node][i] == 1 && !visited[i]) {
            dfs_vector(i, adj_matrix, visited, order);
        }
    }
    order.push_back(node);
}

// kosaraju algorithm for finding strongly connected components
vector<vector<int>> kosaraju_vector(vector<vector<int>>& adj_matrix) {
    // first dfs
    vector<bool> visited(adj_matrix.size(), false);
    vector<int> order;
    for (int i = 0; i < adj_matrix.size(); i++) {
        if (!visited[i]) {
            dfs_vector(i, adj_matrix, visited, order);
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
            dfs_vector(node, adj_matrix_transposed, visited, component);
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
                adj_matrix[i][j] = (rand() % 2)*( rand() % 2);
        }
    }

    return adj_matrix;
}

int main() {
    int n = 10000;
    cout << "gen_adj_matrix\n";
    vector<vector<int>> adj_matrix = gen_adj_matrix(n);

    cout << "kosaraju_deque\n";
    kosaraju_deque(adj_matrix);

    cout << "kosaraju_vector\n";
    kosaraju_vector(adj_matrix);
}