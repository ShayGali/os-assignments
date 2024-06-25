#include <string>
#include <vector>
using std::string;
using std::vector;

vector<vector<int>> kosaraju(vector<vector<int>>& adj_list);

bool add_edge(vector<vector<int>>& g, int i, int j);
bool remove_edge(vector<vector<int>>& g, int i, int j);