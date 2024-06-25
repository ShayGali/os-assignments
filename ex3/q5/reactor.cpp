#include "reactor.hpp"

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
#include <thread>

using std::cout, std::endl;

constexpr int BUF_SIZE = 1024;
constexpr int MAX_CLIENT = 10;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

reactor::reactor() {
    running = false;
    fd_to_func = std::map<int, reactorFunc>();
}

int reactor::add_fd_to_reactor(int fd, reactorFunc func) {
    fd_to_func[fd] = func;
    return 0;
}

int reactor::remove_fd_from_reactor(int fd) {
    if (fd_to_func.find(fd) == fd_to_func.end()) {
        return -1;
    }

    fd_to_func.erase(fd);
    return 0;
}

int reactor::start() {
    if (running) {
        return -1;
    }
    running = true;
    std::thread t1([this] {
        fd_set read_fds;     // temp file descriptor list for select()
        char buf[BUF_SIZE];  // buffer for client data
        int nbytes;
        while (running) {
            // init the fdmax
            int fdmax;
            if (fd_to_func.empty()) {
                fdmax = 0;
            } else {
                fdmax = fd_to_func.rbegin()->first;
            }

            // init the read_fds
            FD_ZERO(&read_fds);

            for (const auto &pair : fd_to_func) {
                FD_SET(pair.first, &read_fds);  // Add each file descriptor to fd_set
            }

            // run through the existing connections looking for data to read
            for (int i = 0; i <= fdmax; i++) {
                if (FD_ISSET(i, &read_fds)) {
                    fd_to_func[i](i);
                }
            }
        }
    });
    return 0;
}

int reactor::stop() {
    running = false;
    return 0;
}