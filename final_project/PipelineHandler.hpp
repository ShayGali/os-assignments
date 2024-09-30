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
 * @brief Command handler implamented using pipeline and active object pattern
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
    template <typename F, typename... Args>
    auto invoke(F &&f, Args &&...args) -> future<decltype(f(args...))> {
        using return_type = decltype(f(args...));  // get the return type of the function
        auto task = make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto result = task->get_future();
        {
            unique_lock<mutex> lock(m);
            tasks.emplace([task]() { (*task)(); });  // add the task to the queue
        }
        cv.notify_one();  // notify the thread that there is a new task
        return result;
    }
};

class PipelineStage : public ActiveObject {
   private:
    function<string(string, int)> task;
    shared_ptr<PipelineStage> next_stage;

   public:
    PipelineStage(function<string(string, int)> task, shared_ptr<PipelineStage> next_stage)
        : task(std::move(task)), next_stage(next_stage) {}

    future<string> process(string input, int user_fd) {
        return invoke([this, input, user_fd] {
            string output = (task(input, user_fd));
            if (next_stage != nullptr) {                              // if there is a next stage
                output = next_stage->process(output, user_fd).get();  // process the output in the next stage
            }
            return output;
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
        return input + "Weight: " + to_string(graph_per_user[user_fd].second.getWeight()) + "\n";
    }

    string mst_longest(string input, int user_fd) {
        return input + "Longest distance: " + to_string(graph_per_user[user_fd].second.longestDist()) + "\n";
    }

    string mst_shortest(string input, int user_fd) {
        return input + "Shortest distance: " + to_string(graph_per_user[user_fd].second.shortestDist()) + "\n";
    }

    string mst_avg(string input, int user_fd) {
        return input + "Average distance: " + to_string(graph_per_user[user_fd].second.avgDist()) + "\n";
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
        string ans = "Got input: " + input;
        istringstream iss(input);
        string command;
        iss >> command;

        std::cout << "client " << user_fd << " sent: " << input << std::endl;

        // update the input to be the rest of the input
        getline(iss, input);

        try {
            if (command == NEW_GRAPH) {
                ans += new_graph_stage->process(input, user_fd).get();
            } else if (command == ADD_EDGE) {
                ans += add_edge_stage->process(input, user_fd).get();
            } else if (command == REMOVE_EDGE) {
                ans += remove_edge_stage->process(input, user_fd).get();
            } else if (command == MST_PRIME || command == MST_KRUSKAL) {
                ans += mst_init_stage->process(command, user_fd).get();
            } else if (command == PRINT_GRAPH) {
                ans += print_graph_stage->process(input, user_fd).get();
            } else {
                ans += "Invalid command";
            }
        } catch (const invalid_argument &e) {
            ans += "Error: " + string(e.what()) + '\n';
        }

        if (ans.back() != '\n') {
            ans += '\n';
        }

        std::cout << "server sent: to client " << user_fd << ": " << ans << std::endl;

        on_end(ans);
    }

    void stop() override {}  // NO NEED TO IMPLEMENT  - stop is implemented in the destructor of the ActiveObject , sherd_ptr will be deleted and the destructor will be called
};