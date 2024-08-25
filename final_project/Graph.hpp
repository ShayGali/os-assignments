#include <vector>

using std::vector;

class Graph {
   private:
    vector<vector<int>> adjMat;

   public:
    Graph(int n);
    Graph(vector<vector<int>>& adjMat);
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    bool isEdge(int u, int v) const;
    vector<int> getNeighbors(int u) const;
    const vector<vector<int>>& getAdjMat() const;
};