#pragma once
#include <iostream>
#include <mutex>

class Guard {
   private:
    std::mutex& mutex;

   public:
    Guard(std::mutex& mutex) : mutex(mutex) {
        std::cout << "Locking mutex" << std::endl;
        mutex.lock();
    }

    ~Guard() {
        std::cout << "Unlocking mutex" << std::endl;
        mutex.unlock();
    }

    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
};