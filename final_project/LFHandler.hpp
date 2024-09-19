#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

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
    auto addTask(F &&task) -> std::future<typename std::result_of<F()>::type> {
        using ReturnType = typename std::result_of<F()>::type;
        auto promise = std::make_shared<std::promise<ReturnType>>();
        auto future = promise->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace([promise, task = std::forward<F>(task)]() mutable {
                try {
                    if constexpr (std::is_void<ReturnType>::value) {
                        task();
                        promise->set_value();
                    } else {
                        promise->set_value(task());
                    }
                } catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        }
        cv.notify_one();
        return future;
    }

    void stop_service() {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }
};

class LFHandler : public CommandHandler {
   private:
    LeaderFollower lf;
    string cmd_handler(string input, int user_fd);

   public:
    LFHandler(map<int, pair<Graph, TreeOnGraph>> &graph_per_user, MSTFactory &mst_factory) : CommandHandler(graph_per_user, mst_factory) {}

    string handle(string input, int user_fd) override {
        auto future = lf.addTask([this, input, user_fd] {
            string ans = cmd_handler(input, user_fd);
            return ans;
        });
        return future.get();  // Wait for the task to complete and get the result
    }

    void stop() override {
        lf.stop_service();
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
            string remaining_input;
            getline(iss, remaining_input);
            ans += init_graph(remaining_input, user_fd);
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
        MSTSolver *solver = mst_factory.createMSTSolver(command);
        TreeOnGraph mst = solver->getMST(g);
        graph_per_user[user_fd].second = mst;
        ans += mst.toString();
        delete solver;
    } else if (command == PRINT_GRAPH) {
        ans = g.toString();
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
