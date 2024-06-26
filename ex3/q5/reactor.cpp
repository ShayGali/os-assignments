#include "reactor.hpp"

#include <iostream>
#include <thread>

using std::cout, std::endl;

constexpr int BUF_SIZE = 1024;
constexpr int MAX_CLIENT = 10;

reactor::reactor() {
    running = false;
    fd_to_func = map<int, reactorFunc>();
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

        // start the reactor
        while (running) {
            // init the fdmax
            int fdmax;
            if (fd_to_func.empty()) {
                fdmax = 0;
            } else {
                fdmax = fd_to_func.rbegin()->first; // Get the last element of the map (rbegin() returns reverse iterator pointing to last element of map), and then take the fd of that element
            }

            // init the read_fds
            FD_ZERO(&read_fds);

            // add the file descriptors to read_fds
            for (const auto &pair : fd_to_func) {
                FD_SET(pair.first, &read_fds);  // Add each file descriptor to fd_set
            }

            // run through the existing connections looking for data to read
            for (int i = 0; i <= fdmax; i++) {
                if (FD_ISSET(i, &read_fds)) {
                    fd_to_func[i](i); // Call the function associated with the file descriptor, and pass the file descriptor as argument
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