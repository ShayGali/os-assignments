#pragma once
#include "Graph.hpp"
#include "TreeOnGraph.hpp"
class MST_Solver {
   public:
    virtual TreeOnGraph getMST(Graph &graph) const = 0;
    virtual ~MST_Solver() {};  // virtual destructor for polymorphism
};

class Kruskal : public MST_Solver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};

class Prim : public MST_Solver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};