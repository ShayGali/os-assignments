#include <string>
#include <vector>
using std::string;
using std::vector;

vector<vector<int>> kosaraju(vector<vector<int>>& adj_list);

bool add_edge(vector<vector<int>>& g, size_t i, size_t j);
bool remove_edge(vector<vector<int>>& g, size_t i, size_t j);