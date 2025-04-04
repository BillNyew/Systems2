package lab2;

public class primefindserial {
  public static boolean isPrime(int n) {
      if (n < 2) return false;
      int i = 2;
      while (i * i <= n) {
          if (n % i == 0) return false;
          i++;
      }
      return true;
  }

  public static int countPrimesInRange(int start, int end) {
      int count = 0;
      for (int i = start; i <= end; i++) {
          if (isPrime(i)) count++;
      }
      return count;
  }

  public static void main(String[] args) {
      int start = 1000, end = 1000000;
      long startTime = System.currentTimeMillis();
      int totalPrimes = countPrimesInRange(start, end);
      long endTime = System.currentTimeMillis();
      System.out.println("Total primes: " + totalPrimes);
      System.out.println("Time taken: " + (endTime - startTime) + " ms");
  }
}
