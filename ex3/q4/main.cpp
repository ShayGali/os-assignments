
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
bool init_graph(vector<vector<int>> &g, istringstream &iss, int user_id);

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
    memset(&hints, 0, sizeof hints);
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
                    addrlen = sizeof remoteaddr;
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
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
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
                        ans= graph_handler(buf, i);
                        for (j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener
                                if (j != listener) {
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
string graph_handler(string input, int user_id) {
    string ans = "input: " + input + "\n";
    vector<vector<int>> g;
    string command;
    pair<int, int> n_m;
    istringstream iss(input);
    int u, v;

    iss >> command;
    if (command == "Newgraph") {
       if(init_graph(g, iss, user_id)){
       ans = "Newgraph created";
    }else{
        exit(1);
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
        if(!(iss >> u >> v)){
            exit(1);
        }
        if (add_edeg(g,u,v)) {
            ans += "Edge added";
        } else {
            ans += "Invalid edge";
        }
    } else if (command == "Removeedge") {
        if(!(iss >> u >> v)){
            exit(1);
        }
        if (remove_edge(g,u,v)) {
            ans += "Edge removed";
        } else {
            ans += "Invalid edge";
        }
    } else {
        ans += "Invalid command";
    }
    if(ans.back() != '\n'){
        ans += "\n";
    }
    return ans;

}

bool init_graph(vector<vector<int>> &g, istringstream &iss, int user_id) {
    char* buf;
    string first, second;
    int n, m, i, u, v, nbytes;
    iss >> n >> m;
    g = vector<vector<int>>(n); 
    i=0;
    while (i < m) {
        if(!(iss >>first >> second)){ // buffer is empty
             if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                return false;
             }
                iss = istringstream(buf);
                continue;
        }
        u = stoi(first);
        v = stoi(second);
        if (u <= 0 || u > n || v <= 0 || v > n || u == v) {
            return false;
        }
        g[u - 1].push_back(v - 1);
        i++; 
        }
    return true;
}
