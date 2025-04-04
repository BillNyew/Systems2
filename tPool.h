#ifndef TPOOL_H
#define TPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <semaphore>
#include <atomic>

using namespace std;  // Ensuring compatibility with poolTester.cc

class tPool {
public:
    explicit tPool(size_t numThreads); // Constructor
    ~tPool(); // Destructor

    void addWork(function<void(void*)> task, void* param); // Adds work to the queue
    void stopPool(); // Stops the thread pool

private:
    void workerThread(int workerID); // Function executed by worker threads

    vector<thread*> workers;  // Worker threads
    queue<function<void(void*)>> taskQueue;  // Work queue
    queue<void*> paramQueue;  // Parameters queue

    mutex queueLock;  // Mutex for protecting queues
    binary_semaphore* workAvailable;  // Signals that work is available
    binary_semaphore* queueEmpty;  // Signals when the queue is empty

    atomic<bool> stopFlag;  // Flag to stop the pool
};

#endif
