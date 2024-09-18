#include <iostream>

#include "Guard.hpp"
#include "Singleton.hpp"

using namespace std;

int main() {
    std::mutex mutex;
    Guard guard(mutex);
    
    // Create a singleton object
    Singleton<int>& singleton = Singleton<int>::getInstance();
    Singleton<string>& singleton_string = Singleton<string>::getInstance();
    // Set the data
    singleton.setData(42);
    singleton_string.setData("Hello, World!");
    Singleton<int>& singleton2 = Singleton<int>::getInstance();
    Singleton<string>& singleton_string2 = Singleton<string>::getInstance();
    cout << "Data: " << singleton2.getData() << endl;
    cout << "Data: " << singleton_string2.getData() << endl;

    // Destroy the singleton object
    Singleton<int>::destroyInstance();
    Singleton<string>::destroyInstance();

    return 0;
}