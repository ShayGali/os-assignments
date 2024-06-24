#include <vector>
#include <string>
using std::vector;
using std::string;


vector<vector<int>> kosaraju(vector<vector<int>>& adj_list);

bool add_edeg(vector<vector<int>>& g, int i, int j);
bool remove_edge(vector<vector<int>>& g, int i, int j);