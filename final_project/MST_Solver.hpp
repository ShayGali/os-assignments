#pragma once
#include "Graph.hpp"
#include "TreeOnGraph.hpp"
class MST_Solver {
   private:
   public:
    virtual TreeOnGraph getMST(Graph &graph) const = 0;
};

class Kruskal : public MST_Solver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};

class Prim : public MST_Solver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};