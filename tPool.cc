#include "tPool.h"
#include <iostream>
#include <chrono>
#include <random>

using namespace std;  // Ensuring compatibility with poolTester.cc

tPool::tPool(size_t numThreads) : stopFlag(false) {
    // Initialize semaphores
    workAvailable = new binary_semaphore(0);
    queueEmpty = new binary_semaphore(1);

    // Create worker threads
    for (size_t i = 0; i < numThreads; i++) {
        workers.push_back(new thread(&tPool::workerThread, this, i));
    }
}

tPool::~tPool() {
    stopPool();
    delete workAvailable;
    delete queueEmpty;
}

// Producer: Adds work to the queue
void tPool::addWork(function<void(void*)> task, void* param) {
    {
        lock_guard<mutex> lock(queueLock);  // Lock the queue

        // **PRINT HERE BEFORE ADDING WORK TO QUEUE**
        cout << "-->Adding work " << endl;

        taskQueue.push(task);
        paramQueue.push(param);

        // If queue was empty before, mark it as non-empty
        queueEmpty->try_acquire();
    } // Mutex is automatically unlocked here

    // Signal that work is available
    workAvailable->release();
}

// Consumer: Worker threads execute tasks
void tPool::workerThread(int workerID) {
    cout << "Worker " << workerID << " started\n";

    while (!stopFlag) {
        workAvailable->acquire();  // Wait for work

        queueLock.lock();  // Lock the queue
        if (taskQueue.empty()) {
            queueLock.unlock();
            continue;
        }

        // Retrieve task
        function<void(void*)> task = taskQueue.front();
        void* param = paramQueue.front();
        taskQueue.pop();
        paramQueue.pop();

        // If queue is now empty, signal it
        if (taskQueue.empty()) {
            queueEmpty->release();
        }
        queueLock.unlock();

        // Execute the task
        cout << "Worker " << workerID << " about to do work\n";
        task(param);
    }

    cout << "Worker " << workerID << " stopping\n";
}

// Stops the thread pool
void tPool::stopPool() {
    // Wait until all work is done
    queueEmpty->acquire();

    // Set the stop flag
    stopFlag = true;

    // Wake up all worker threads
    for (size_t i = 0; i < workers.size(); i++) {
        workAvailable->release();
    }

    // Join all worker threads
    for (thread* th : workers) {
        th->join();
        delete th;
    }

    workers.clear();
}
