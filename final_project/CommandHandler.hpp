
#include <iostream>
#include <map>

#include "Graph.hpp"
#include "MSTFactory.hpp"
#include "TreeOnGraph.hpp"
#include "client_commands.hpp"

using std::istringstream;
using std::map;
using std::string;

class CommandHandler {
   public:
    CommandHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory);
    virtual string handle(string input, int user_fd) = 0;
    virtual void stop() = 0;

   protected:
    map<int, pair<Graph, TreeOnGraph>> &graph_per_user;
    MSTFactory &mst_factory;
};
