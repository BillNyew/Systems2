#include <iostream>
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

int countPrimesInRange(int start, int end) {
    int count = 0;
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) count++;
    }
    return count;
}

int main() {
    int start = 1000, end = 1000000;
    clock_t startTime = clock();
    int totalPrimes = countPrimesInRange(start, end);
    clock_t endTime = clock();
    std::cout << "Total primes: " << totalPrimes << std::endl;
    std::cout << "Time taken: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << std::endl;
    return 0;
}
