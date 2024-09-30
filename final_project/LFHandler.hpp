#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

#include "CommandHandler.hpp"

constexpr int MAX_THREADS = 4;
class LeaderFollower {
   private:
    std::queue<std::function<void()>> taskQueue;  // queue of tasks that the threads will execute
    std::mutex mutex;                             // mutex the queue
    std::condition_variable cv;                   // condition variable to notify the threads that there is a new task or
    std::atomic<bool> stop;                       // flag to stop the threads
    std::vector<std::thread> threads;             // vector of threads

   public:
    LeaderFollower() : stop(false) {
        for (size_t i = 0; i < MAX_THREADS; ++i) {  // create the threads and run the workerThread function
            threads.emplace_back(&LeaderFollower::workerThread, this);
        }
    }

    ~LeaderFollower();

    void addTask(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(mutex);  // add the task to the queue and notify one of the threads
        taskQueue.push(std::move(task));
        cv.notify_one();
    }

    void stop_work() {
        stop.store(true);
        cv.notify_all();
    }

   private:
    void workerThread();
};

class LFHandler : public CommandHandler {
   private:
    LeaderFollower lf;  // Leader-Follower pattern

    string cmd_handler(string input, int user_fd);

   public:
    LFHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory)
        : CommandHandler(graph_per_user, mst_factory) {
    }

    /**
     * Handle the input command
     * will get a string input and a user file descriptor.
     * add new task to the Leader-Follower pattern
     */
    void handle(string input, int user_fd, function<void(string)> on_end) override {
        lf.addTask([this, input, user_fd, on_end]() {
            string ans = cmd_handler(input, user_fd);  // handle the command
            on_end(ans);                               // call the on_end function with the result
        });
    }

    void stop_work() override {
        lf.stop_work();
    }
};
