#include "MinHeap.hpp"

#include <climits>

MinHeap::MinHeap(int capacity) : capacity(capacity), size(0) {
    heap.resize(capacity);
    pos.resize(capacity);
    key.resize(capacity, INT_MAX);
}

void MinHeap::insertKey(int v, int key) {
    size++;
    int i = size - 1;
    heap[i] = v;
    pos[v] = i;
    this->key[v] = key;

    while (i != 0 && this->key[heap[(i - 1) / 2]] > this->key[heap[i]]) {
        swap(heap[i], heap[(i - 1) / 2]);
        pos[heap[i]] = i;
        pos[heap[(i - 1) / 2]] = (i - 1) / 2;
        i = (i - 1) / 2;
    }
}

void MinHeap::decreaseKey(int v, int key) {
    int i = pos[v];
    this->key[v] = key;

    while (i != 0 && this->key[heap[(i - 1) / 2]] > this->key[heap[i]]) {
        swap(heap[i], heap[(i - 1) / 2]);
        pos[heap[i]] = i;
        pos[heap[(i - 1) / 2]] = (i - 1) / 2;
        i = (i - 1) / 2;
    }
}

int MinHeap::extractMin() {
    if (size == 0) {
        return -1;
    }

    int root = heap[0];

    if (size > 1) {
        heap[0] = heap[size - 1];
        pos[heap[0]] = 0;
        heapify(0);
    }

    size--;
    return root;
}

bool MinHeap::isInMinHeap(int v) {
    return pos[v] < size;
}

bool MinHeap::isEmpty() {
    return size == 0;
}

void MinHeap::heapify(int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < size && key[heap[left]] < key[heap[smallest]]) {
        smallest = left;
    }

    if (right < size && key[heap[right]] < key[heap[smallest]]) {
        smallest = right;
    }

    if (smallest != idx) {
        swap(heap[idx], heap[smallest]);
        pos[heap[idx]] = idx;
        pos[heap[smallest]] = smallest;
        heapify(smallest);
    }
}

void MinHeap::swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}