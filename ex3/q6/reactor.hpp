#include <functional>
#include <map>
using std::map;

typedef function<void(int)> reactorFunc;

class reactor {
   private:
    map < int, f bool running;

   public:
    reactor();
    int add_fd_to_reactor(int fd, reactorFunc func);
    int remove_fd_from_reactor(int fd);

    int start();
    int stop();
};