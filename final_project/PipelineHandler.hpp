#pragma once

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

    void run();

   public:
    ActiveObject() {
        my_thread = thread([this] { run(); });
    }

    ~ActiveObject();

    void stop_work();

    /**
     * @brief invoke a function in the active object
     */
    void invoke(function<void()> task);
};

class PipelineStage : public ActiveObject {
   private:
    function<string(string, int)> task;
    shared_ptr<PipelineStage> next_stage;

   public:
    PipelineStage(function<string(string, int)> task, shared_ptr<PipelineStage> next_stage)
        : task(std::move(task)), next_stage(next_stage) {}

    void process(string input, int user_fd, function<void(string)> on_end);
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

    string add_edge(string input, int user_fd);
    string remove_edge(string input, int user_fd);
    string mst_init(string input, int user_fd);
    string mst_weight(string input, int user_fd);
    string mst_longest(string input, int user_fd);
    string mst_shortest(string input, int user_fd);
    string mst_avg(string input, int user_fd);

   public:
    PipelineHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory);

    void handle(string input, int user_fd, function<void(string)> on_end) override;

    void stop_work() override;
};