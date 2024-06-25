#include "reactor.hpp"

reactor::reactor() {
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
    return 0;
}