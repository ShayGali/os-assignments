
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

#include "CommandHandler.hpp"
#include "Graph.hpp"
#include "LFHandler.hpp"
#include "MSTFactory.hpp"
#include "PipelineHandler.hpp"
#include "client_commands.hpp"

using namespace std;

// global variable for the graph
// Graph g(0);
// TreeOnGraph mst(g);
MSTFactory mst_factory;

map<int, pair<Graph, TreeOnGraph>> graph_per_user;

// Define constants for buffer size, port, and max clients
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
        cout << "\033[34m" << "selectserver: new connection from " << client_ip << " on socket " << newfd << "\033[0m" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    CommandHandler *handler = nullptr;
    int opt;
    // check if we get l flag or p flag (l for LFHandler, p for PipelineHandler)
    while ((opt = getopt(argc, argv, "lp")) != -1) {
        switch (opt) {
            case 'l':
                if (handler != nullptr) {
                    delete handler;
                    cerr << "You can't use both flags" << endl;
                    exit(EXIT_FAILURE);
                }
                handler = new LFHandler(graph_per_user, mst_factory);
                break;
            case 'p':
                if (handler != nullptr) {
                    delete handler;
                    cerr << "You can't use both flags" << endl;
                    exit(EXIT_FAILURE);
                }
                handler = new PipelineHandler(graph_per_user, mst_factory);
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-l] [-p]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (handler == nullptr) {
        // display warning message (in yellow)
        cerr << "\033[33m" << "[WARNING] No flag was given, using LFHandler by default" << "\033[0m" << endl;
        handler = new LFHandler(graph_per_user, mst_factory);
    }

    // variables for the server
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

                    // create a new graph for the user
                    graph_per_user[i] = make_pair(Graph(), TreeOnGraph());
                } else {  // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0 || errno == ECONNRESET) {
                            cout << "\033[34m" << "selectserver: socket " << i << " hung up" << "\033[0m" << std::endl;
                        } else {
                            perror("recv");
                        }
                        close(i);            // closing the socket of the client
                        FD_CLR(i, &master);  // remove from master set
                    } else {                 // we got some data from a client
                        // add '\0' to the end of the buffer
                        buf[nbytes] = '\0';

                        if (string(buf).starts_with("kill")) {
                            cout << "\033[33m" << "Server got kill command from client " << i << "\033[0m" << endl;
                            // stop the handler
                            handler->stop();
                            // close all the sockets
                            for (int j = 0; j <= fdmax; j++) {
                                if (FD_ISSET(j, &master)) {
                                    close(j);
                                }
                            }
                            // free the memory
                            delete handler;
                            // stop the server
                            close(listener);
                            cout << "\033[33m" << "Server is shutting down" << "\033[0m" << endl;
                            return 0;
                        }

                        ans = handler->handle(buf, i);
                        if (send(i, ans.c_str(), ans.size(), 0) == -1) {
                            perror("send");
                        }
                        memset(buf, 0, sizeof(buf));  // clear the buffer
                    }
                }  // END handle data from client
            }  // END got new incoming connection
        }  // END looping through file descriptors
    }  // END for(;;)--and you thought it would never end!
    return 0;
}
