
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

using namespace std;

// Define constants for buffer size, port, and max clients
constexpr int BUF_SIZE = 1024;
constexpr char PORT[] = "3490";
constexpr int MAX_CLIENT = 10;

string graph_handler(string input, int user_id);
void init_graph(vector<vector<int>> &g, istringstream &iss, int user_id);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void) {
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;                        // listening socket descriptor
    int newfd;                           // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr;  // client address
    socklen_t addrlen;

    char buf[BUF_SIZE];  // buffer for client data
    int nbytes;
    string ans;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);  // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        cerr << "selectserver: " << gai_strerror(rv) << std::endl;
        exit(1);
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
        exit(2);
    }

    freeaddrinfo(ai);  // all done with this

    // listen
    if (listen(listener, MAX_CLIENT) == -1) {
        perror("listen");
        exit(3);
    }

    cout << "selectserver: waiting for connections on port " << PORT << endl;

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener;  // so far, it's this one

    // main loop
    for (;;) {
        read_fds = master;  // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {  // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof(remoteaddr);
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master);  // add to master set
                        if (newfd > fdmax) {     // keep track of the max
                            fdmax = newfd;
                        }

                        const char *client_ip = inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
                        cout << "selectserver: new connection from " << client_ip << " on socket " << newfd << std::endl;
                    }
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
                        ans = graph_handler(buf, i);
                        for (j = 0; j <= fdmax; j++) {
                            // send to  what client i do
                            if (FD_ISSET(j, &master)) {
                                if (j != listener) {  // dont send to listener (we send to client i also)
                                    if (send(j, ans.c_str(), ans.size(), 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }  // END handle data from client
            }  // END got new incoming connection
        }  // END looping through file descriptors
    }  // END for(;;)--and you thought it would never end!
    return 0;
}

vector<vector<int>> g;

string graph_handler(string input, int user_id) {
    string ans = "Got input: " + input + " from user " + to_string(user_id) + "\n";
    string command;
    pair<int, int> n_m;
    istringstream iss(input);
    int u, v;

    iss >> command;
    if (command == "Newgraph") {
        try {
            init_graph(g, iss, user_id);
            ans += "Newgraph created";
        } catch (exception &e) {
            ans += e.what();
        }
    } else if (command == "Kosaraju") {
        vector<vector<int>> components = kosaraju(g);
        for (int i = 0; i < components.size(); i++) {
            ans += "Component " + to_string(i) + ": ";
            for (int j = 0; j < components[i].size(); j++) {
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

    return ans;
}

void init_graph(vector<vector<int>> &g, istringstream &iss, int user_id) {
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
}
