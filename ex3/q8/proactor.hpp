#include <functional>
#include <map>
#include <mutex>
#include <thread>

using std::function;
using std::map;
using std::mutex;
using std::thread;

typedef function<void*(int)> proactorFunc;

class proactor {
   private:
    mutex lock;
};