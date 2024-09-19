#pragma once
#include "Graph.hpp"
#include "TreeOnGraph.hpp"
class MSTSolver {
   public:
    virtual TreeOnGraph getMST(Graph &graph) const = 0;
    virtual ~MSTSolver() {};  // virtual destructor for polymorphism
};

class Kruskal : public MSTSolver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};

class Prim : public MSTSolver {
   public:
    TreeOnGraph getMST(Graph &graph) const override;
};