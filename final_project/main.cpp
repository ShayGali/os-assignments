
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
#include <map>
#include <sstream>
#include <vector>

#include "Graph.hpp"
#include "MSTFactory.hpp"
#include "client_commands.hpp"

using namespace std;

// global variable for the graph
// Graph g(0);
// TreeOnGraph mst(g);
MST_Factory mst_factory;

map<int, pair<Graph, TreeOnGraph>> graph_per_user;

// Define constants for buffer size, port, and max clients
constexpr int BUF_SIZE = 1024;
constexpr char PORT[] = "9034";
constexpr int MAX_CLIENT = 10;

string command_handler(string input, int user_fd);
string init_graph(istringstream &iss, int user_fd);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int open_server() {
    int listener;  // listening socket descriptor

    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // get us a socket and bind it
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "selectserver: " << gai_strerror(rv) << endl;
        exit(EXIT_FAILURE);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // allow socket descriptor to be reuseable
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        cerr << "selectserver: failed to bind\n";
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(ai);  // all done with this

    // listen
    if (listen(listener, MAX_CLIENT) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "selectserver: waiting for connections on port " << PORT << endl;

    return listener;
}

void accept_connection(int listener, fd_set &master, int &fdmax) {
    struct sockaddr_storage remoteaddr;  // client address
    socklen_t addrlen = sizeof(remoteaddr);
    int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        FD_SET(newfd, &master);  // add to master set
        if (newfd > fdmax) {     // keep track of the max
            fdmax = newfd;
        }

        char remoteIP[INET6_ADDRSTRLEN];
        const char *client_ip = inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
        cout << "selectserver: new connection from " << client_ip << " on socket " << newfd << std::endl;
    }
}

int main(void) {
    // variables for the server
    struct sockaddr_storage remoteaddr;  // client address
    int newfd;                           // newly accept()ed socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];
    char buf[BUF_SIZE];  // buffer for client data
    int nbytes;
    string ans;

    int listener = open_server();

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    FD_ZERO(&master);  // clear the master and temp sets
    FD_ZERO(&read_fds);

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener;  // so far, it's this one

    // main loop
    for (;;) {
        read_fds = master;  // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // run through the existing connections looking for data to read
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {  // we got one!!
                if (i == listener) {
                    // handle new connections
                    accept_connection(listener, master, fdmax);
                } else {  // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            cout << "selectserver: socket " << i << " hung up" << std::endl;
                        } else {
                            perror("recv");
                        }
                        close(i);            // closing the socket of the client
                        FD_CLR(i, &master);  // remove from master set
                    } else {                 // we got some data from a client
                        // add '\0' to the end of the buffer
                        buf[nbytes] = '\0';
                        ans = command_handler(buf, i);
                        // TODO: send the answer to the client
                        if (send(i, ans.c_str(), ans.size(), 0) == -1) {
                            perror("send");
                        }
                    }
                }  // END handle data from client
            }  // END got new incoming connection
        }  // END looping through file descriptors
    }  // END for(;;)--and you thought it would never end!
    return 0;
}

string command_handler(string input, int user_fd) {
    string ans = "Got input: " + input;
    string command;
    istringstream iss(input);
    int u, v, w;

    Graph &g = graph_per_user[user_fd].first;

    iss >> command;
    if (command == NEW_GRAPH) {
        try {
            ans += init_graph(iss, user_fd);
            ans += "New graph created";
        } catch (exception &e) {
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
        } catch (exception &e) {
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
        } catch (exception &e) {
            ans += e.what();
        }
    } else if (command == MST_PRIME || command == MST_KRUSKAL) {
        MST_Solver *solver = mst_factory.createMSTSolver(command);
        TreeOnGraph mst = solver->getMST(g);
        graph_per_user[user_fd].second = mst;
        ans += mst.toString();
        delete solver;
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

/**
 * @brief Initialize the graph with the given number of vertices and edges
 * expected input: n m u1 v1 w1 u2 v2 w2 ... um vm wm
 */
string init_graph(istringstream &iss, int user_fd) {
    char buf[BUF_SIZE] = {0};
    string first, second, third, send_data;
    int n, m, i, u, v, w, nbytes;
    if (!(iss >> n >> m)) {
        throw invalid_argument("Invalid input - expected n and m");
    }
    Graph temp(n);
    i = 0;
    while (i < m) {
        if (!(iss >> first >> second >> third)) {  // buffer is empty (we assume that we dont have the first in the buffer, we need to get both of them)
            if ((nbytes = recv(user_fd, buf, sizeof(buf), 0)) <= 0) {
                throw invalid_argument("Invalid input - you dont send the " + to_string(i + 1) + " edge");
            }
            send_data += buf;
            iss = istringstream(buf);
            continue;
        }
        // convert string to int
        u = stoi(first);
        v = stoi(second);
        w = stoi(third);

        // check if u and v are valid
        if (u <= 0 || u > n || v <= 0 || v > n || u == v) {
            throw invalid_argument("Invalid input - invalid edge. Edge must be between 1 and " + to_string(n) + " and u != v. Got: " + first + " " + second);
        }

        if (w <= 0) {
            throw invalid_argument("Invalid input - invalid weight. Weight must be greater than 0. Got: " + third);
        }

        // add edge to the graph
        temp.addEdge(u - 1, v - 1, w);
        temp.addEdge(v - 1, u - 1, w);
        i++;
    }

    // if we reach here, we have a valid graph
    graph_per_user[user_fd].first = temp;

    return send_data;
}