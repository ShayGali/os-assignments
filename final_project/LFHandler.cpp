#include "LFHandler.hpp"

void LeaderFollower::workerThread() {
    while (!stop.load()) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return stop.load() || !taskQueue.empty(); });
            if (stop.load() && taskQueue.empty()) {
                return;
            }
            task = std::move(taskQueue.front());  // get the task from the queue
            taskQueue.pop();                      // remove the task from the queue
        }
        task();  // execute the task
    }
}

LeaderFollower::~LeaderFollower() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop.store(true);
        cv.notify_all();
    }
    for (auto &thread : threads) {
        thread.join();
    }
}

/**
 * Handle the input command
 */
string LFHandler::cmd_handler(string input, int user_fd) {
    string ans = "Got input: " + input;
    string command;
    istringstream iss(input);
    int u, v, w;

    iss >> command;

    // std::cout << "client " << user_fd << " sent: " << input << std::endl;

    try {
        if (command == NEW_GRAPH) {
            string remaining_input;
            getline(iss, remaining_input);
            ans += init_graph(remaining_input, user_fd);
        } else if (command == ADD_EDGE) {
            // get u and v from the input
            if (!(iss >> u >> v >> w)) {  // if the buffer is empty we throw an error
                ans += "Invalid input - expected format: u v w\n";
                return ans;
            }
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                Graph &g = graph_per_user[user_fd].first;
                g.addEdge(u - 1, v - 1, w);
                g.addEdge(v - 1, u - 1, w);
            }

        } else if (command == REMOVE_EDGE) {
            if (!(iss >> u >> v)) {
                ans += "Invalid input - expected u and v\n";
                return ans;
            }
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                Graph &g = graph_per_user[user_fd].first;
                g.removeEdge(u - 1, v - 1);
                g.removeEdge(v - 1, u - 1);
            }
        } else if (command == MST_PRIME || command == MST_KRUSKAL) {
            MSTSolver *solver = mst_factory.createMSTSolver(command);
            TreeOnGraph mst;
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                Graph &g = graph_per_user[user_fd].first;
                mst = solver->getMST(g);
                graph_per_user[user_fd].second = mst;
            }

            ans += "MST: \n" + mst.toString();

            // do calculations
            // add weight
            ans += "Weight: " + std::to_string(mst.getWeight()) + "\n";
            // add longest distance
            ans += "Longest distance: " + std::to_string(mst.longestDist()) + "\n";
            // add shortest distance
            ans += "Shortest distance: " + std::to_string(mst.shortestDist()) + "\n";
            // add average distance
            ans += "Average distance: " + std::to_string(mst.avgDist()) + "\n";
            delete solver;
        } else if (command == PRINT_GRAPH) {
            {
                std::lock_guard<std::mutex> lock(graph_mutex);
                ans += graph_per_user[user_fd].first.toString();
            }
        } else {
            ans += "Invalid command";
        }
    } catch (const std::exception &e) {
        ans += "Error: " + string(e.what());
    }

    // ad LF if needed
    if (ans.back() != '\n') {
        ans += "\n";
    }

    // std::cout << "server sent: to client " << user_fd << ": " << ans << std::endl;

    return ans;
}