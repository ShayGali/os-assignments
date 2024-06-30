#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
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

string graph_handler(string input, int user_id);
string init_graph(vector<vector<int>> &g, istringstream &iss, int user_id);
void client_handler(int user_id, mutex &lock);

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
    proactor_obj.start_proactor(listener_fd, client_handler);
}

void client_handler(int user_id, mutex &mtx) {
    char buf[BUF_SIZE] = {0};
    int nbytes;
    string ans;
    // while we receive data
    while ((nbytes = recv(user_id, buf, sizeof(buf), 0)) > 0) {
        string input = buf;
        mtx.lock();
        cout << "mutex locked by client" << user_id << endl;
        ans = graph_handler(input, user_id);
        mtx.unlock();
        cout << ans << "mutex unlocked\n " << endl;
        send(user_id, ans.c_str(), ans.size(), 0);
        buf[0] = '\0';
    }
    close(user_id);
    cout << "Connection closed for client " << user_id << endl;
}

string graph_handler(string input, int user_id) {
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
        for (size_t i = 0; i < components.size(); i++) {
            ans += "Component " + to_string(i) + ": ";
            for (size_t j = 0; j < components[i].size(); j++) {
                ans += to_string(components[i][j] + 1) + " ";
            }
            ans += "\n";
        }
    } else if (command == "Newedge") {
        // get u and v from the input
        if (!(iss >> u >> v)) {  // if the buffer is empty we throw an error
            exit(1);
        }
        if (add_edge(g, u, v)) {
            ans += "Edge added";
        } else {
            ans += "Invalid edge";
        }
    } else if (command == "Removeedge") {
        if (!(iss >> u >> v)) {
            exit(1);
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
