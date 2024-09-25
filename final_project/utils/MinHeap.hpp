#pragma once
#include <vector>
class MinHeap {
   public:
    MinHeap(int capacity);
    void insertKey(int v, int key);
    void decreaseKey(int v, int key);
    int extractMin();
    bool isInMinHeap(int v);
    bool isEmpty();

   private:
    std::vector<int> heap;
    std::vector<int> pos;
    std::vector<int> key;
    int capacity;
    int size;

    void heapify(int idx);
    void swap(int &a, int &b);
};