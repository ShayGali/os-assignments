#pragma once
#include <mutex>

template <typename T>
class Singleton {
   private:
    static Singleton<T>* instance;
    static std::mutex mutex;

    T data;

    Singleton() = default;

   public:
    static Singleton<T>& getInstance();
    static void destroyInstance();

    T getData() const { return data; }
    void setData(T data) { this->data = data; }

    // Delete copy constructor and assignment operator
    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;
};

// initialize the instance to nullptr

template <typename T>
Singleton<T>* Singleton<T>::instance = nullptr;

template <typename T>
std::mutex Singleton<T>::mutex;

template <typename T>
Singleton<T>& Singleton<T>::getInstance() {
    if (instance == nullptr) {                    // Check if the instance is null
        std::lock_guard<std::mutex> lock(mutex);  // Lock the mutex
        if (instance == nullptr) {                // Check if the instance is still null
            instance = new Singleton<T>();        // Create a new instance
        }
        // Unlock the mutex
        std::lock_guard<std::mutex> unlock(mutex);
    }
    return *instance;  // Return reference to the instance
}

template <typename T>
void Singleton<T>::destroyInstance() {
    // Lock the mutex
    std::lock_guard<std::mutex> lock(mutex);
    if (instance != nullptr) {  // Check if the instance is not null
        delete instance;        // Delete the instance
        instance = nullptr;     // Set the instance to null
    }
    // Unlock the mutex
    std::lock_guard<std::mutex> unlock(mutex);
}
