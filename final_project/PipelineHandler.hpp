#pragma once
#include <sys/socket.h>
#include <unistd.h>

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "CommandHandler.hpp"

using std::condition_variable;
using std::function;
using std::future;
using std::invalid_argument;
using std::istringstream;
using std::make_shared;
using std::mutex;
using std::queue;
using std::shared_ptr;
using std::string;
using std::thread;
using std::to_string;
using std::unique_lock;

constexpr int BUF_SIZE = 1024;

/**
 * @brief Command handler implamented using pipeline and active object pattern
 *
 */

class ActiveObject {
   private:
    queue<function<void()>> tasks;
    thread thread;
    mutex m;
    condition_variable cv;
    bool stop = false;

    void run() {
        while (!stop) {
            function<void()> task;
            {
                unique_lock<mutex> lock(m);
                cv.wait(lock, [this] { return !tasks.empty() || stop; });
                if (tasks.empty()) {
                    continue;
                }
                task = tasks.front();
                tasks.pop();
            }
            task();
        }
    }

   public:
    ActiveObject() : thread([this] { run(); }) {}

    ~ActiveObject() {
        {
            unique_lock<mutex> lock(m);
            stop = true;
        }
        cv.notify_all();
        thread.join();
    }

    template <typename F, typename... Args>
    auto invoke(F &&f, Args &&...args) -> future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        auto task = make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto result = task->get_future();
        {
            unique_lock<mutex> lock(m);
            tasks.emplace([task]() { (*task)(); });
        }
        cv.notify_one();
        return result;
    }
};

class PipelineStage : public ActiveObject {
   private:
    shared_ptr<PipelineStage> next_stage;
    function<string(string, int)> task;

   public:
    PipelineStage(function<string(string, int)> task, shared_ptr<PipelineStage> next_stage) : task(std::move(task)), next_stage(next_stage) {}

    future<string> process(string input, int user_fd) {
        return invoke([this, input, user_fd] {
            string output = (task(input, user_fd));
            if (next_stage != nullptr) {
                next_stage->process(output, user_fd);
            }
            return output;
        });
    }
};

class PipelineHandler : public CommandHandler {
   private:
    shared_ptr<PipelineStage> new_graph_stage;
    shared_ptr<PipelineStage> add_edge_stage;
    shared_ptr<PipelineStage> remove_edge_stage;
    shared_ptr<PipelineStage> mst_init_stage;
    shared_ptr<PipelineStage> mst_weight_stage;
    shared_ptr<PipelineStage> mst_longest_stage;
    shared_ptr<PipelineStage> mst_shortest_stage;
    shared_ptr<PipelineStage> mst_avg_stage;

    string add_edge(string input, int user_fd) {
        istringstream iss(input);
        int u, v, w;
        if (!(iss >> u >> v >> w)) {
            throw invalid_argument("Invalid input - expected u, v, and w");
        }
        if (u <= 0 || u > graph_per_user[user_fd].first.V() || v <= 0 || v > graph_per_user[user_fd].first.V() || u == v) {
            throw invalid_argument("Invalid input - invalid edge. Edge must be between 1 and " + to_string(graph_per_user[user_fd].first.V()) + " and u != v. Got: " + to_string(u) + " " + to_string(v));
        }

        if (w <= 0) {
            throw invalid_argument("Invalid input - invalid weight. Weight must be greater than 0. Got: " + to_string(w));
        }

        graph_per_user[user_fd].first.addEdge(u - 1, v - 1, w);
        graph_per_user[user_fd].first.addEdge(v - 1, u - 1, w);

        return "edge added\n";
    }

    string remove_edge(string input, int user_fd) {
        istringstream iss(input);
        int u, v;
        if (!(iss >> u >> v)) {
            throw invalid_argument("Invalid input - expected u and v");
        }
        if (u <= 0 || u > graph_per_user[user_fd].first.V() || v <= 0 || v > graph_per_user[user_fd].first.V() || u == v) {
            throw invalid_argument("Invalid input - invalid edge. Edge must be between 1 and " + to_string(graph_per_user[user_fd].first.V()) + " and u != v. Got: " + to_string(u) + " " + to_string(v));
        }

        graph_per_user[user_fd].first.removeEdge(u - 1, v - 1);
        graph_per_user[user_fd].first.removeEdge(v - 1, u - 1);

        return "edge removed\n";
    }

    string mst_init(string input, int user_fd) {
        MSTSolver *solver = mst_factory.createMSTSolver(input);
        TreeOnGraph mst = solver->getMST(graph_per_user[user_fd].first);
        graph_per_user[user_fd].second = mst;
        delete solver;
        return mst.toString();
    }

    string mst_weight(string input, int user_fd) {
        return input + to_string(graph_per_user[user_fd].second.getWeight()) + "\n";
    }

    string mst_longest(string input, int user_fd) {
        return input + to_string(graph_per_user[user_fd].second.longestDist()) + "\n";
    }

    string mst_shortest(string input, int user_fd) {
        return input + to_string(graph_per_user[user_fd].second.shortestDist()) + "\n";
    }

    string mst_avg(string input, int user_fd) {
        return input + to_string(graph_per_user[user_fd].second.avgDist()) + "\n";
    }

   public:
    PipelineHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory) : CommandHandler(graph_per_user, mst_factory) {
        new_graph_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return init_graph(input, user_fd); }, nullptr);
        add_edge_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return add_edge(input, user_fd); }, nullptr);
        remove_edge_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return remove_edge(input, user_fd); }, nullptr);
        mst_init_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_init(input, user_fd); }, mst_weight_stage);
        mst_weight_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_weight(input, user_fd); }, mst_longest_stage);
        mst_longest_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_longest(input, user_fd); }, mst_shortest_stage);
        mst_shortest_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_shortest(input, user_fd); }, mst_avg_stage);
        mst_avg_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_avg(input, user_fd); }, nullptr);
    }

    string handle(string input, int user_fd) override {
        istringstream iss(input);
        string command;
        iss >> command;
        if (command == NEW_GRAPH) {
            return new_graph_stage->process(input, user_fd).get();
        } else if (command == ADD_EDGE) {
            return add_edge_stage->process(input, user_fd).get();
        } else if (command == REMOVE_EDGE) {
            return remove_edge_stage->process(input, user_fd).get();
        } else if (command == MST_PRIME || command == MST_KRUSKAL) {
            return mst_init_stage->process(command, user_fd).get();
        } else {
            return "Invalid command\n";
        }
    }

    void stop() override {}  // NO NEED TO IMPLEMENT  - stop is implemented in the destructor of the ActiveObject , sherd_ptr will be deleted and the destructor will be called
};