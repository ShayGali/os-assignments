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

#include "reactor.hpp"

using namespace std;

constexpr int BUF_SIZE = 1024;
constexpr char PORT[] = "3490";
constexpr int MAX_CLIENT = 10;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main() {
    reactor reactor;

    int listener_fd;
    struct sockaddr_storage remoteaddr;  // client address
    socklen_t addrlen;

    char buf[BUF_SIZE];  // buffer for client data
    int nbytes;
    string ans;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;  // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

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

    reactor.add_fd_to_reactor(listener_fd, [](int fd) {
        struct sockaddr_storage remoteaddr;  // client address
        socklen_t addrlen;

        char buf[BUF_SIZE];  // buffer for client data
        int nbytes;
        string ans;

        char remoteIP[INET6_ADDRSTRLEN];

        int newfd;
        struct sockaddr_storage their_addr;
        socklen_t sin_size;

        sin_size = sizeof(their_addr);
        newfd = accept(fd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
        } else {
            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), remoteIP, INET6_ADDRSTRLEN);
            cout << "selectserver: got connection from " << remoteIP << endl;

            nbytes = recv(newfd, buf, sizeof(buf), 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                    cout << "selectserver: socket " << newfd << " hung up" << endl;
                } else {
                    perror("recv");
                }
                close(newfd);
            } else {
                buf[nbytes] = '\0';
                cout << "selectserver: " << buf << endl;
                ans = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                send(newfd, ans.c_str(), ans.size(), 0);
            }
        }
    });
}
