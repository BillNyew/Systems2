package lab2;

import java.util.ArrayList;

class PrimeThread extends Thread {
    private int start, end;
    private int[] results;
    private int index;

    public PrimeThread(int start, int end, int[] results, int index) {
        this.start = start;
        this.end = end;
        this.results = results;
        this.index = index;
    }

    @Override
    public void run() {
        int count = 0;
        for (int i = start; i <= end; i++) {
            if (primefinderthread.isPrime(i)) count++;
        }
        results[index] = count;
        System.out.println("Thread " + index + " finished: " + count + " primes in range [" + start + ", " + end + "]");
    }
}

public class primefinderthread {
    public static boolean isPrime(int n) {
        if (n < 2) return false;
        int i = 2;
        while (i * i <= n) {
            if (n % i == 0) return false;
            i++;
        }
        return true;
    }

    public static void main(String[] args) throws InterruptedException {
        int start = 1000, end = 1000000;
        int threadsCount = 4;
        int range = (end - start + 1) / threadsCount;
        int[] results = new int[threadsCount];
        ArrayList<Thread> threads = new ArrayList<>();

        long startTime = System.currentTimeMillis();
        for (int i = 0; i < threadsCount; i++) {
            int rangeStart = start + i * range;
            int rangeEnd = (i == threadsCount - 1) ? end : rangeStart + range - 1;
            Thread thread = new PrimeThread(rangeStart, rangeEnd, results, i);
            threads.add(thread);
            thread.start();
        }

        for (Thread thread : threads) {
            thread.join();
        }

        int totalPrimes = 0;
        for (int result : results) totalPrimes += result;

        long endTime = System.currentTimeMillis();
        System.out.println("Total primes: " + totalPrimes);
        System.out.println("Time taken: " + (endTime - startTime) + " ms");
    }
}
