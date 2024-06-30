#include <functional>
#include <map>
#include <thread>

using std::function;
using std::map;
using std::thread;

typedef function<void*(int)> reactorFunc;

class reactor {
   private:
    bool running;
    map<int, reactorFunc> fd_to_func;
    thread eventLoopThread;

   public:
    reactor();
    ~reactor();
    int add_fd_to_reactor(int fd, reactorFunc func);
    int remove_fd_from_reactor(int fd);

    int start();
    int stop();
};