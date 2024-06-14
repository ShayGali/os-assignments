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

pair<int, int> get_pair_from_input() {
    string line;
    int a, b;
    if (!getline(cin, line)) {
        cerr << "Invalid input format" << endl;
        exit(1);
    }
    if (sscanf(line.c_str(), "%d %d", &a, &b) != 2) {
        cerr << "Invalid input format" << endl;
        exit(1);
    }
    return make_pair(a, b);
}

void with_adj_matrix() {
    pair<int, int> n_m = get_pair_from_input();
    int n = n_m.first;
    int m = n_m.second;

    // init matrix n x n
    vector<vector<int>> adj_matrix(n, vector<int>(n, 0));

    // read edges (u v\n means u -> v)
    for (int i = 0; i < m; i++) {
        pair<int, int> edge = get_pair_from_input();
        // check if edge is valid
        if (edge.first <= 0 || edge.first > n || edge.second <= 0 || edge.second > n) {
            cerr << "Invalid edge" << endl;
            exit(1);
        }
        adj_matrix[edge.first - 1][edge.second - 1] = 1;
    }

    vector<vector<int>> components = kosaraju_adj_mat(adj_matrix);
    for (int i = 0; i < components.size(); i++) {
        cout << "Component " << i << ": ";
        for (int j = 0; j < components[i].size(); j++) {
            cout << (components[i][j] + 1) << " ";
        }
        cout << endl;
    }
}

void with_adj_list() {
    pair<int, int> n_m = get_pair_from_input();
    int n = n_m.first;
    int m = n_m.second;

    // init n lists
    vector<vector<int>> adj_list(n);

    // read edges (u v\n means u -> v)
    for (int i = 0; i < m; i++) {
        pair<int, int> edge = get_pair_from_input();
        // check if edge is valid
        if (edge.first <= 0 || edge.first > n || edge.second <= 0 || edge.second > n) {
            cerr << "Invalid edge" << endl;
            exit(1);
        }
        adj_list[edge.first - 1].push_back(edge.second - 1);
    }

    vector<vector<int>> components = kosaraju_adj_list(adj_list);
    for (int i = 0; i < components.size(); i++) {
        cout << "Component " << i << ": ";
        for (int j = 0; j < components[i].size(); j++) {
            cout << (components[i][j] + 1) << " ";
        }
        cout << endl;
    }
}
int main() {
    /*
    5 5
    1 2
    2 3
    3 1
    3 4
    4 5
    */

   cout << "with_adj_list\n";
    with_adj_list();
    cout << "with_adj_matrix\n";
    with_adj_matrix();
}