#include <iostream>
#include <thread>
#include <vector>
#include <cmath>

bool isPrime(int n) {
    if (n < 2) return false;
    int i = 2;
    while (i * i <= n) {
        if (n % i == 0) return false;
        i++;
    }
    return true;
}

void countPrimesInRange(int start, int end, int& result) {
    int count = 0;
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) count++;
    }
    result = count;
    std::cout << "Thread finished range [" << start << ", " << end << "] with " << count << " primes.\n";
}

int main() {
    int start = 1000, end = 1000000, threadsCount = 4;
    int range = (end - start + 1) / threadsCount;

    std::vector<int> results(threadsCount, 0);
    std::vector<std::thread> threads;

    clock_t startTime = clock();
    for (int i = 0; i < threadsCount; i++) {
        int rangeStart = start + i * range;
        int rangeEnd = (i == threadsCount - 1) ? end : rangeStart + range - 1;
        threads.emplace_back(countPrimesInRange, rangeStart, rangeEnd, std::ref(results[i])); // create a malloc or new   }

    for (auto& t : threads) {
        t.join();
    }

    int totalPrimes = 0;
    for (int result : results) totalPrimes += result;

    clock_t endTime = clock();
    std::cout << "Total primes: " << totalPrimes << std::endl;
    std::cout << "Time taken: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << std::endl;
    return 0;
}
