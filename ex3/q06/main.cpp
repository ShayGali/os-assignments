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

#include "kosaraju.hpp"
#include "reactor.hpp"

using namespace std;

constexpr int BUF_SIZE = 1024;
constexpr char PORT[] = "3490";
constexpr int MAX_CLIENT = 10;

// Define the graph as a global variable
vector<vector<int>> g;

string graph_handler(string input, int user_id);
string init_graph(vector<vector<int>> &g, istringstream &iss, int user_id);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
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

    return listener_fd;
}

int main() {
    reactor reactor_obj;
    int listener_fd = get_listener_fd();

    cout << "selectserver: waiting for connections on port " << PORT << endl;
    // create the listener_fd, and add it to the reactor_obj
    reactor_obj.add_fd_to_reactor(listener_fd, [&reactor_obj](int fd) -> void * {
        char remoteIP[INET6_ADDRSTRLEN];

        int newfd;
        struct sockaddr_storage their_addr;
        socklen_t sin_size;

        sin_size = sizeof(their_addr);
        newfd = accept(fd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            return nullptr;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), remoteIP, INET6_ADDRSTRLEN);
        cout << "selectserver: got connection from " << remoteIP << endl;

        // add the newfd to the reactor_obj, and set the callback function
        reactor_obj.add_fd_to_reactor(newfd, [&reactor_obj](int fd) -> void * {
            char buf[BUF_SIZE];  // buffer for client data
            int nbytes;
            nbytes = recv(fd, buf, sizeof(buf), 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                    cout << "selectserver: socket " << fd << " hung up" << endl;
                } else {
                    perror("recv");
                }
                close(fd);
                reactor_obj.remove_fd_from_reactor(fd);
                return nullptr;
            } else {
                buf[nbytes] = '\0';
                string res = graph_handler(buf, fd);
                send(fd, res.c_str(), res.size(), 0);  // send the result back to the client
            }
            return nullptr;
        });
        return nullptr;
    });

    reactor_obj.start();

    return 0;
}

string graph_handler(string input, int user_id) {
    string ans;
    string command;
    pair<int, int> n_m;
    istringstream iss(input);
    int u, v;

    iss >> command;
    if (command == "Newgraph") {
        try {
            ans += init_graph(g, iss, user_id);
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

    return ans;
}

string init_graph(vector<vector<int>> &g, istringstream &iss, int user_id) {
    char buf[BUF_SIZE] = {0};
    string first, second;
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
    return "New graph created";
}
