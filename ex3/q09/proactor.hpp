#include <functional>
#include <map>
#include <mutex>
#include <thread>

using std::function;
using std::map;
using std::mutex;
using std::thread;

typedef function<void(int, mutex&)> proactorFunc;

class proactor {
   private:
    mutex mtx;
    thread eventLoopThread;

   public:
    void start_proactor(int listener, proactorFunc client_handler);
    void stop_proactor();
    ~proactor();
};