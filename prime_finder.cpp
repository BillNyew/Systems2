// g++ -fopenmp prime_finder.cpp -o prime_finder
// time ./prime_finder
#include <iostream>
#include <omp.h>
using namespace std;

bool isPrime(long long number) {
    if (number < 2) return false;
    for (long long divisor = 2; divisor * divisor <= number; divisor++) {
        if (number % divisor == 0) return false;
    }
    return true;
}

void findPrimesBlocking(long long start, long long end, int numThreads) {
    cout << "Blocking\n";
    omp_set_num_threads(numThreads);
    double overallStart = omp_get_wtime();

    double threadTimes[numThreads] = {0.0};
    int threadLocalCounts[numThreads] = {0};
    int totalPrimes = 0;

    #pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        double threadStartTime = omp_get_wtime();

        #pragma omp for schedule(static) reduction(+:totalPrimes)
        for (long long num = start; num <= end; num++) {
            if (isPrime(num)) {
                totalPrimes++;
                threadLocalCounts[threadID]++;
            }
        }

        double threadEndTime = omp_get_wtime();
        threadTimes[threadID] = threadEndTime - threadStartTime;
    }

    for (int i = 0; i < numThreads; i++) {
        cout << "  time for " << i << ": " << threadTimes[i] << " with " << threadLocalCounts[i] << " found\n";
    }

    double overallEnd = omp_get_wtime();
    cout << "overall time: " << (overallEnd - overallStart) << " with " << totalPrimes << " found\n\n";
}

void findPrimesStriping(long long start, long long end, int numThreads) {
    cout << "Striping \n";
    omp_set_num_threads(numThreads);
    double overallStart = omp_get_wtime();

    double threadTimes[numThreads] = {0.0};
    int threadLocalCounts[numThreads] = {0};
    int totalPrimes = 0;

    #pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        double threadStartTime = omp_get_wtime();

        #pragma omp for scheduling(static, 1) reduction(+:totalPrimes)
        for (long long num = start; num <= end; num++) {
            if (isPrime(num)) {
                totalPrimes++;
                threadLocalCounts[threadID]++;
            }
        }

        double threadEndTime = omp_get_wtime();
        threadTimes[threadID] = threadEndTime - threadStartTime;
    }

    for (int i = 0; i < numThreads; i++) {
        cout << "  time for " << i << ": " << threadTimes[i] << " with " << threadLocalCounts[i] << " found\n";
    }

    double overallEnd = omp_get_wtime();
    cout << "overall time: " << (overallEnd - overallStart) << " with " << totalPrimes << " found\n\n";
}

int main() {
    long long start = 1000;
    long long end = 1000000; // adjust this range if needed to get ~15-60s runtime
    int numThreads = 5;

    findPrimesBlocking(start, end, numThreads);
    findPrimesStriping(start, end, numThreads);

    return 0;
}