#include "reactor.hpp"

reactor::reactor() {
    fd_to_func = std::map<int, reactorFunc>();
}