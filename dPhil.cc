// Compile: g++ dPhil.cc -std=c++20 -pthread -o dPhil
// Run: ./dPhil
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
using namespace std;

int numPhil;
int thinkingMax = 10;
int eatingMax = 100;
binary_semaphore* screenLock;
binary_semaphore** chopstickLocks;
char* stickHolding;

// Resource graph globals
int numRes;
int numProc;
int** resourceGraph;

int left(int philNum) {
    return (philNum+1)%numPhil;
}
int right(int philNum) {
    return philNum;
}

void displaySticks() {
    screenLock->acquire();
    cout << "[ ";
    for (int i=0; i<numPhil; i++) {
        cout << stickHolding[i] << " ";
    }
    cout << " ]\n";
    screenLock->release();
}

void think(int philNum) {
    int duration = (rand()%thinkingMax)+1;
    screenLock->acquire();
    cout << philNum << " is thinking for " << duration << "ms\n";
    screenLock->release();
    this_thread::sleep_for(chrono::milliseconds(rand()%duration));
}

void eat(int philNum) {
    int duration = (rand()%eatingMax)+1;
    screenLock->acquire();
    cout << philNum << " is EATING for " << duration << "ms\n";
    screenLock->release();
    this_thread::sleep_for(chrono::milliseconds(rand()%duration));
}

// Deadlock cycle detection helpers
bool processDFS(int proc, bool visited[], bool recStack[]);
bool resourceDFS(int res, bool visited[], bool recStack[]);

bool processDFS(int proc, bool visited[], bool recStack[]) {
    if (recStack[proc]) return true;
    if (visited[proc]) return false;
//omp4 with schedule and reduction
    visited[proc] = true;
    recStack[proc] = true;

    for (int res = 0; res < numRes; res++) {
        if (resourceGraph[proc][res] == 1) {
            if (resourceDFS(res, visited, recStack)) return true;
        }
    }

    recStack[proc] = false;
    return false;
}

bool resourceDFS(int res, bool visited[], bool recStack[]) {
    for (int proc = 0; proc < numProc; proc++) {
        if (resourceGraph[proc][res] == -1) {
            if (processDFS(proc, visited, recStack)) return true;
        }
    }
    return false;
}

int deadlockCheck() {
    bool visited[numProc] = {false};
    bool recStack[numProc] = {false};

    for (int proc = 0; proc < numProc; proc++) {
        if (!visited[proc]) {
            if (processDFS(proc, visited, recStack)) {
                return 1;
            }
        }
    }
    return 0;
}

// Resource graph acquire and release
void rgAcquire(int proc, int res, binary_semaphore* sem) {
    resourceGraph[proc][res] = 1; // request

    if (deadlockCheck() == 1) {
        cout << "Deadlock detected (before acquire of resource " << res << " by process " << proc << ")\n";
    }

    sem->acquire();
    resourceGraph[proc][res] = -1; // acquired
}

void rgRelease(int proc, int res, binary_semaphore* sem) {
    resourceGraph[proc][res] = 0;
    sem->release();
}

void takeLeftStick(int philNum) {
    rgAcquire(philNum, left(philNum), chopstickLocks[left(philNum)]);
    stickHolding[philNum] = 'L';
    displaySticks();
}

void takeRightStick(int philNum) {
    rgAcquire(philNum, right(philNum), chopstickLocks[right(philNum)]);
    stickHolding[philNum] = 'B';
    displaySticks();
}

void putSticks(int philNum) {
    screenLock->acquire();
    cout << " " << philNum << " returning chopsticks\n";
    screenLock->release();
    stickHolding[philNum] = '-';
    displaySticks();

    rgRelease(philNum, left(philNum), chopstickLocks[left(philNum)]);
    rgRelease(philNum, right(philNum), chopstickLocks[right(philNum)]);
}

void philosopher(int philNum) {
    while (true) {
        think(philNum);
        takeLeftStick(philNum);
        takeRightStick(philNum);
        eat(philNum);
        putSticks(philNum);
    }
}

// Commented out testing graph code for final integration
/*
void deadlockTester() {
    numProc = 7;
    numRes = 6;

    resourceGraph = new int*[numProc];
    for (int i=0; i<numProc; i++) {
        resourceGraph[i] = new int[numRes];
        for (int j=0; j<numRes; j++) {
            resourceGraph[i][j] = 0;
        }
    }

    resourceGraph[0][0] = -1;  // R->A
    resourceGraph[0][1] = +1;  // A->S
    resourceGraph[1][2] = +1;  // B->T
    resourceGraph[2][1] = +1;  // C->S
    resourceGraph[3][1] = +1;  // D->S
    resourceGraph[3][2] = +1;  // D->T
    resourceGraph[3][3] = -1;  // U->D
    resourceGraph[4][2] = -1;  // T->E
    resourceGraph[4][4] = +1;  // E->V
    resourceGraph[5][1] = +1;  // F->S
    resourceGraph[5][5] = -1;  // W->F
    resourceGraph[6][3] = +1;  // G->U
    resourceGraph[6][4] = -1;  // V->G

    cout << "Resource Graph Test:\n";
    for (int i = 0; i < numProc; i++) {
        for (int j = 0; j < numRes; j++) {
            cout << resourceGraph[i][j] << " ";
        }
        cout << endl;
    }

    if (deadlockCheck() == 1) {
        cout << "Deadlock detected in test setup\n";
    } else {
        cout << "No deadlock detected in test setup\n";
    }
}
*/

int main() {
    srand(0);
    screenLock = new binary_semaphore(1);

    numPhil = 5;
    numProc = numPhil;
    numRes = numPhil;

    resourceGraph = new int*[numProc];
    for (int i = 0; i < numProc; i++) {
        resourceGraph[i] = new int[numRes];
        for (int j = 0; j < numRes; j++) {
            resourceGraph[i][j] = 0;
        }
    }

    chopstickLocks = new binary_semaphore*[numPhil];
    for (int i=0; i<numPhil; i++) {
        chopstickLocks[i] = new binary_semaphore(1);
    }

    stickHolding = new char[numPhil];
    for (int i=0; i<numPhil; i++) {
        stickHolding[i] = '-';
    }

    // Commented out test invocation:
    // deadlockTester();
    // exit(0);

    thread* ths[numPhil];
    for (int i=0; i<numPhil; i++) {
        thread* thPhil = new thread(philosopher, i);
        ths[i] = thPhil;
    }

    for (int i=0; i<numPhil; i++) {
        ths[i]->join();
    }
}
