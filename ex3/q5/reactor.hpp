#include <functional>
#include <map>

using std::function;
using std::map;

typedef function<void(int)> reactorFunc;

class reactor {
   private:
    map < int, reactorFunc > fd_to_func;

   public:
    reactor();
    int add_fd_to_reactor(int fd, reactorFunc func);
    int remove_fd_from_reactor(int fd);

    int start();
    int stop();
};