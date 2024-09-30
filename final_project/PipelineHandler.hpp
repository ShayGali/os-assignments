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

/**
 * @brief Command handler
 *
 */

class ActiveObject {
   private:
    queue<function<void()>> tasks;  // queue of tasks
    thread my_thread;               // thread that will run the tasks
    mutex m;                        // mutex for the tasks queue
    condition_variable cv;          // condition variable to notify the thread that there is a new task
    bool stop = false;              // flag to stop the thread

    void run() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lock(m);
                cv.wait(lock, [this] { return !tasks.empty() || stop; });
                if (stop && tasks.empty()) {
                    return;
                }
                if (!tasks.empty()) {
                    task = tasks.front();
                    tasks.pop();
                }
            }
            if (task) {
                task();
            }
        }
    }

   public:
    ActiveObject() {
        my_thread = thread([this] { run(); });
    }

    ~ActiveObject() {
        {
            unique_lock<mutex> lock(m);
            stop = true;
            cv.notify_all();  // Move notify_all() inside the lock
        }
        my_thread.join();
    }

    /**
     * @brief invoke a function in the active object
     */
    void invoke(function<void()> task) {
        unique_lock<mutex> lock(m);
        tasks.push(task);
        cv.notify_one();
    }
};

class PipelineStage : public ActiveObject {
   private:
    function<string(string, int)> task;
    shared_ptr<PipelineStage> next_stage;

   public:
    PipelineStage(function<string(string, int)> task, shared_ptr<PipelineStage> next_stage)
        : task(std::move(task)), next_stage(next_stage) {}

    void process(string input, int user_fd, function<void(string)> on_end) {
        invoke([this, input, user_fd, on_end] {
            try {
                string output = (task(input, user_fd));
                if (next_stage != nullptr) {                       // if there is a next stage
                    next_stage->process(output, user_fd, on_end);  // process the output in the next stage
                } else {
                    on_end(output);
                }
            } catch (const invalid_argument &e) {
                on_end(string(e.what()) + "\n");
            }
        });
    }
};

class PipelineHandler : public CommandHandler {
   private:
    // stages of the pipeline
    shared_ptr<PipelineStage> new_graph_stage;
    shared_ptr<PipelineStage> add_edge_stage;
    shared_ptr<PipelineStage> remove_edge_stage;
    shared_ptr<PipelineStage> mst_init_stage;
    shared_ptr<PipelineStage> mst_weight_stage;
    shared_ptr<PipelineStage> mst_longest_stage;
    shared_ptr<PipelineStage> mst_shortest_stage;
    shared_ptr<PipelineStage> mst_avg_stage;
    shared_ptr<PipelineStage> print_graph_stage;

    string add_edge(string input, int user_fd) {
        istringstream iss(input);
        int u, v, w;
        if (!(iss >> u >> v >> w)) {
            throw invalid_argument("Invalid input - expected u, v, and w");
        }

        graph_mutex.lock();
        graph_per_user[user_fd].first.addEdge(u - 1, v - 1, w);
        graph_per_user[user_fd].first.addEdge(v - 1, u - 1, w);
        graph_mutex.unlock();

        return "edge added\n";
    }

    string remove_edge(string input, int user_fd) {
        istringstream iss(input);
        int u, v;
        if (!(iss >> u >> v)) {
            throw invalid_argument("Invalid input - expected u and v");
        }

        graph_mutex.lock();
        graph_per_user[user_fd].first.removeEdge(u - 1, v - 1);
        graph_per_user[user_fd].first.removeEdge(v - 1, u - 1);
        graph_mutex.unlock();

        return "edge removed\n";
    }

    string mst_init(string input, int user_fd) {
        MSTSolver *solver = mst_factory.createMSTSolver(input);
        graph_mutex.lock();
        TreeOnGraph mst = solver->getMST(graph_per_user[user_fd].first);
        graph_per_user[user_fd].second = mst;
        graph_mutex.unlock();
        delete solver;
        return mst.toString();
    }

    string mst_weight(string input, int user_fd) {
        graph_mutex.lock();
        // copy the mst
        TreeOnGraph mst = graph_per_user[user_fd].second;
        graph_mutex.unlock();

        return input + "Weight: " + to_string(mst.getWeight()) + "\n";
    }

    string mst_longest(string input, int user_fd) {
        graph_mutex.lock();
        // copy the mst
        TreeOnGraph mst = graph_per_user[user_fd].second;
        graph_mutex.unlock();
        return input + "Longest distance: " + to_string(mst.longestDist()) + "\n";
    }

    string mst_shortest(string input, int user_fd) {
        graph_mutex.lock();
        // copy the mst
        TreeOnGraph mst = graph_per_user[user_fd].second;
        graph_mutex.unlock();
        return input + "Shortest distance: " + to_string(mst.shortestDist()) + "\n";
    }

    string mst_avg(string input, int user_fd) {
        graph_mutex.lock();
        // copy the mst
        TreeOnGraph mst = graph_per_user[user_fd].second;
        graph_mutex.unlock();
        return input + "Average distance: " + to_string(mst.avgDist()) + "\n";
    }

   public:
    PipelineHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory)
        : CommandHandler(graph_per_user, mst_factory), new_graph_stage(nullptr), add_edge_stage(nullptr), remove_edge_stage(nullptr), mst_init_stage(nullptr), mst_weight_stage(nullptr), mst_longest_stage(nullptr), mst_shortest_stage(nullptr), mst_avg_stage(nullptr), print_graph_stage(nullptr) {
        // create the stages of the pipeline
        new_graph_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return init_graph(input, user_fd); }, nullptr);
        add_edge_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return add_edge(input, user_fd); }, nullptr);
        remove_edge_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return remove_edge(input, user_fd); }, nullptr);
        mst_avg_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_avg(input, user_fd); }, nullptr);
        mst_shortest_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_shortest(input, user_fd); }, mst_avg_stage);
        mst_longest_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_longest(input, user_fd); }, mst_shortest_stage);
        mst_weight_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_weight(input, user_fd); }, mst_longest_stage);
        mst_init_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return mst_init(input, user_fd); }, mst_weight_stage);
        print_graph_stage = make_shared<PipelineStage>([this](string input, int user_fd) { return this->graph_per_user[user_fd].first.toString(); }, nullptr);
    }

    void handle(string input, int user_fd, function<void(string)> on_end) override {
        istringstream iss(input);
        string command;
        iss >> command;

        // update the input to be the rest of the input
        getline(iss, input);

        if (command == NEW_GRAPH) {
            new_graph_stage->process(input, user_fd, on_end);
        } else if (command == ADD_EDGE) {
            add_edge_stage->process(input, user_fd, on_end);
        } else if (command == REMOVE_EDGE) {
            remove_edge_stage->process(input, user_fd, on_end);
        } else if (command == MST_PRIME || command == MST_KRUSKAL) {
            mst_init_stage->process(command, user_fd, on_end);
        } else if (command == PRINT_GRAPH) {
            print_graph_stage->process(input, user_fd, on_end);
        } else {
            on_end("Invalid command\n");
        }
    }
};