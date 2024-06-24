#include <vector>
using std::vector;

string graph_handler(string input);

vector<vector<int>> kosaraju(vector<vector<int>>& adj_list);

vector<vector<int>> get_input_graph(int n, int m);
bool add_edeg(vector<vector<int>>& g, int i, int j);
bool remove_edge(vector<vector<int>>& g, int i, int j);