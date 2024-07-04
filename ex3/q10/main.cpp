#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sstream>
#include <vector>

#include "kosaraju.hpp"
#include "proactor.hpp"

using namespace std;

constexpr int BUF_SIZE = 1024;
constexpr char PORT[] = "3490";
constexpr int MAX_CLIENT = 10;

// Define the graph as a global variable
vector<vector<int>> g;
condition_variable more_than_50;
bool more_than_50_flag = false;

string graph_handler(string input, int user_id, mutex &mtx);
string init_graph(vector<vector<int>> &g, istringstream &iss, int user_id);
void client_handler(int user_id, mutex &mtx);

/**
 * this function is use by the thread that will print the status of the kosaraju function
 * the thread will wait until the condition variable is notified and then print the status
 * after he notify, he will wait again until the next notification (but now he will print the status always)
 */
void print_kosaraju_status(mutex &mtx) {
    // wait until the more_than_50_flag is true, and then each time the kosaraju function is called we will print the status
    unique_lock<mutex> lk(mtx);
    more_than_50.wait(lk, [] { return more_than_50_flag; });
    while (true) {
        // check if the flag is still true
        if (more_than_50_flag) {
            cout << "\033[1;32m" << "At Least 50% of the graph belongs to the same scc\n\n"
                 << "\033[0m";
        } else {
            cout << "\033[1;33m" << "At Least 50% of the graph no longer belongs to the same scc\n\n"
                 << "\033[0m";
        }

        // wint until the next notification
        more_than_50.wait(lk);
    }
}
int get_listener_fd() {
    int listener_fd;
    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    struct addrinfo hints, *ai, *p;
    int rv;

    // get us a socket and bind it
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "selectserver: " << gai_strerror(rv) << endl;
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener_fd < 0) {
            continue;
        }

        // allow socket descriptor to be reuseable
        setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener_fd, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener_fd);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        cerr << "selectserver: failed to bind\n";
        exit(2);
    }

    freeaddrinfo(ai);  // all done with this

    // listen
    if (listen(listener_fd, MAX_CLIENT) == -1) {
        perror("listen");
        exit(3);
    }

    cout << "selectserver: waiting for connections on port " << PORT << endl;

    return listener_fd;
}

int main() {
    proactor proactor_obj;

    int listener_fd = get_listener_fd();

    // create a theread that will print the status of the kosaraju (just when the condition variable is notified)
    thread kosaraju_thread(print_kosaraju_status, ref(proactor_obj.get_lock()));

    kosaraju_thread.detach();

    proactor_obj.start_proactor(listener_fd, client_handler);
}

void client_handler(int user_id, mutex &mtx) {
    char buf[BUF_SIZE] = {0};
    int nbytes;
    string ans;
    // while we receive data
    while ((nbytes = recv(user_id, buf, sizeof(buf), 0)) > 0) {
        buf[nbytes] = '\0';  // add null terminator to the end of the buffer
        string input = buf;
        mtx.lock();
        cout << "mutex locked by client" << user_id << endl;
        ans = graph_handler(input, user_id, mtx);
        mtx.unlock();
        cout << ans << "mutex unlocked\n " << endl;
        send(user_id, ans.c_str(), ans.size(), 0);
        buf[0] = '\0';
    }
    close(user_id);
    cout << "Connection closed for client " << user_id << endl;
}

string graph_handler(string input, int user_id, mutex &mtx) {
    string ans = "Got input: " + input;
    string command;
    pair<int, int> n_m;
    istringstream iss(input);
    int u, v;

    iss >> command;
    if (command == "Newgraph") {
        try {
            ans += init_graph(g, iss, user_id);
            ans += "New graph created";
        } catch (exception &e) {
            ans += e.what();
        }
    } else if (command == "Kosaraju") {
        vector<vector<int>> components = kosaraju(g);
        size_t n = g.size();        // number of nodes in the graph
        more_than_50_flag = false;  // init the flag to false, change it to true if needed
        for (size_t i = 0; i < components.size(); i++) {
            // if some component have more than 50% of the nodes wake up the thread that is waiting on the condition variable
            if (components[i].size() > n / 2) {
                more_than_50_flag = true;
            }

            ans += "Component " + to_string(i) + ": ";
            for (size_t j = 0; j < components[i].size(); j++) {
                ans += to_string(components[i][j] + 1) + " ";
            }
            ans += "\n";
        }
        // notify the thread that is waiting on the condition variable (or wake up the thread if it is waiting after the first wake up)
        more_than_50.notify_one();
    } else if (command == "Newedge") {
        // get u and v from the input
        if (!(iss >> u >> v)) {  // if the buffer is empty we throw an error
            ans += "Invalid input - expected u and v\n";
            return ans;
        }
        if (add_edge(g, u, v)) {
            ans += "Edge added";
        } else {
            ans += "Invalid edge";
        }
    } else if (command == "Removeedge") {
        if (!(iss >> u >> v)) {
            ans += "Invalid input - expected u and v\n";
            return ans;
        }
        if (remove_edge(g, u, v)) {
            ans += "Edge removed";
        } else {
            ans += "Invalid edge";
        }
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

string init_graph(vector<vector<int>> &g, istringstream &iss, int user_id) {
    char buf[BUF_SIZE] = {0};
    string first, second, send_data;
    int n, m, i, u, v, nbytes;
    if (!(iss >> n >> m)) {
        throw invalid_argument("Invalid input - expected n and m");
    }
    vector<vector<int>> temp(n);
    i = 0;
    while (i < m) {
        if (!(iss >> first >> second)) {  // buffer is empty (we assume that we dont have the first in the buffer, we need to get both of them)
            if ((nbytes = recv(user_id, buf, sizeof(buf), 0)) <= 0) {
                throw invalid_argument("Invalid input - you dont send the " + to_string(i + 1) + " edge");
            }
            send_data += buf;
            iss = istringstream(buf);
            continue;
        }
        // convert string to int
        u = stoi(first);
        v = stoi(second);

        // check if u and v are valid
        if (u <= 0 || u > n || v <= 0 || v > n || u == v) {
            throw invalid_argument("Invalid input - invalid edge. Edge must be between 1 and " + to_string(n) + " and u != v. Got: " + first + " " + second);
        }
        // add edge to the graph
        temp[u - 1].push_back(v - 1);
        i++;
    }
    g = temp;
    return send_data;
}
