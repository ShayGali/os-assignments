#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include "Graph.hpp"
#include "MSTFactory.hpp"
#include "TreeOnGraph.hpp"
#include "client_commands.hpp"

using std::function;
using std::istringstream;
using std::map;
using std::string;

class CommandHandler {
   public:
    CommandHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory) : graph_per_user(graph_per_user), mst_factory(mst_factory) {}
    virtual ~CommandHandler() = default;
    virtual void handle(string input, int user_fd, function<void(string)> on_end) = 0;
    virtual void stop() = 0;

   protected:
    map<int, pair<Graph, TreeOnGraph>> &graph_per_user;
    MSTFactory &mst_factory;

    /**
     * @brief Initialize the graph with the given number of vertices and edges
     * expected input: n m u1 v1 w1 u2 v2 w2 ... um vm wm
     */
    string init_graph(string input, int user_fd) {
        istringstream iss(input);
        string first, second, third, send_data;
        int n, m, i, u, v, w;
        if (!(iss >> n >> m)) {
            throw std::invalid_argument("Invalid input - expected n and m");
        }
        Graph temp(n);
        i = 0;
        while (i < m) {
            if (!(iss >> first >> second >> third)) {  // buffer is empty (we assume that we dont have the first in the buffer, we need to get both of them)
                throw std::invalid_argument("Invalid input - you dont send the " + std::to_string(i + 1) + " edge");
                continue;
            }
            // convert string to int
            u = stoi(first);
            v = stoi(second);
            w = stoi(third);

            // check if u and v are valid
            if (u <= 0 || u > n || v <= 0 || v > n || u == v) {
                throw std::invalid_argument("Invalid input - invalid edge. Edge must be between 1 and " + std::to_string(n) + " and u != v. Got: " + first + " " + second);
            }

            if (w <= 0) {
                throw std::invalid_argument("Invalid input - invalid weight. Weight must be greater than 0. Got: " + third);
            }

            // add edge to the graph
            temp.addEdge(u - 1, v - 1, w);
            temp.addEdge(v - 1, u - 1, w);
            i++;
        }

        // if we reach here, we have a valid graph
        graph_per_user[user_fd].first = temp;

        return send_data;
    }
};
