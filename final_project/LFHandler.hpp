#pragma once

#include <sys/socket.h>

#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>

#include "command_handler.hpp"

using std::istringstream;
using std::string;

constexpr int BUF_SIZE = 1024;

constexpr int MAX_THREADS = 4;

class LeaderFollower {
   private:
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    bool stop;

    void workerThread() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return !tasks.empty() || stop; });

            if (stop && tasks.empty()) {
                return;
            }

            // Leader takes a task
            auto task = std::move(tasks.front());
            tasks.pop();

            // Leader becomes a follower
            lock.unlock();

            // Execute the task
            task();
        }
    }

   public:
    LeaderFollower() : stop(false) {
        for (size_t i = 0; i < MAX_THREADS; ++i) {
            threads.emplace_back(&LeaderFollower::workerThread, this);
        }
    }

    ~LeaderFollower() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        cv.notify_all();
        for (auto &thread : threads) {
            thread.join();
        }
    }

    template <typename F>
    auto addTask(F &&task) -> std::future<decltype(task())> {
        auto promise = std::make_shared<std::promise<decltype(task())>>();
        auto future = promise->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace([promise, task = std::forward<F>(task)]() mutable {
                try {
                    promise->set_value(task());
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        }
        cv.notify_one();
        return future;
    }
};

class LFHandler : public CommandHandler {
   private:
    LeaderFollower lf;
    string cmd_handler(string input, int user_fd);
    string init_graph(istringstream &iss, int user_fd);

   public:
    LFHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MST_Factory &mst_factory) : CommandHandler(graph_per_user, mst_factory) {}

    string handle(string input, int user_fd) override {
        auto future = lf.addTask([this, input, user_fd] {
            string ans = cmd_handler(input, user_fd);
            if (send(user_fd, ans.c_str(), ans.size(), 0) == -1) {
                perror("send");
            }
            return ans;
        });
        return future.get();  // Wait for the task to complete and get the result
    }

    void stop() override {
        lf.addTask([this] {
            CommandHandler::stop();
        });
    }
};

string LFHandler::cmd_handler(string input, int user_fd) {
    string ans = "Got input: " + input;
    string command;
    istringstream iss(input);
    int u, v, w;

    Graph &g = graph_per_user[user_fd].first;

    iss >> command;
    if (command == NEW_GRAPH) {
        try {
            ans += init_graph(iss, user_fd);
            ans += "New graph created";
        } catch (std::exception &e) {
            ans += e.what();
        }
    } else if (command == ADD_EDGE) {
        // get u and v from the input
        if (!(iss >> u >> v >> w)) {  // if the buffer is empty we throw an error
            ans += "Invalid input - expected format: u v w\n";
            return ans;
        }
        try {
            g.addEdge(u - 1, v - 1, w);
            g.addEdge(v - 1, u - 1, w);
        } catch (std::exception &e) {
            ans += e.what();
        }
    } else if (command == REMOVE_EDGE) {
        if (!(iss >> u >> v)) {
            ans += "Invalid input - expected u and v\n";
            return ans;
        }
        try {
            g.removeEdge(u - 1, v - 1);
            g.removeEdge(v - 1, u - 1);
        } catch (std::exception &e) {
            ans += e.what();
        }
    } else if (command == MST_PRIME || command == MST_KRUSKAL) {
        MST_Solver *solver = mst_factory.createMSTSolver(command);
        TreeOnGraph mst = solver->getMST(g);
        graph_per_user[user_fd].second = mst;
        ans += mst.toString();
        delete solver;
    } else {
        ans += "Invalid command";
    }

    // ad LF if needed
    if (ans.back() != '\n') {
        ans += "\n";
    }
    iss.clear();
    return ans;
}
/**
 * @brief Initialize the graph with the given number of vertices and edges
 * expected input: n m u1 v1 w1 u2 v2 w2 ... um vm wm
 */
string LFHandler::init_graph(istringstream &iss, int user_fd) {
    char buf[BUF_SIZE] = {0};
    string first, second, third, send_data;
    int n, m, i, u, v, w, nbytes;
    if (!(iss >> n >> m)) {
        throw std::invalid_argument("Invalid input - expected n and m");
    }
    Graph temp(n);
    i = 0;
    while (i < m) {
        if (!(iss >> first >> second >> third)) {  // buffer is empty (we assume that we dont have the first in the buffer, we need to get both of them)
            if ((nbytes = recv(user_fd, buf, sizeof(buf), 0)) <= 0) {
                throw std::invalid_argument("Invalid input - you dont send the " + std::to_string(i + 1) + " edge");
            }
            send_data += buf;
            iss = istringstream(buf);
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
