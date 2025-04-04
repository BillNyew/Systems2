import java.io.*;
import java.net.*;

public class Node {
    private static final String HEAD_IP = "127.0.0.1"; // Change if Head is on another machine
    private static final int PORT = 5000;

    public static void main(String[] args) {
        new Node().startNode();
    }

    public void startNode() {
        try (Socket socket = new Socket(HEAD_IP, PORT);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));    //
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true)) {             //send it as an object not strings
     
            System.out.println("Connected to Head at " + HEAD_IP);
            String range = in.readLine();
            System.out.println("Received range: " + range);

            String[] parts = range.split(" ");
            int start = Integer.parseInt(parts[0]);
            int end = Integer.parseInt(parts[1]);

            String result = findPrimes(start, end);
            out.println(result);
            System.out.println("Sent results back to Head.");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private String findPrimes(int start, int end) {
        StringBuilder primes = new StringBuilder();
        for (int num = start; num <= end; num++) {
            if (isPrime(num)) {
                primes.append(num).append(",");
            }
        }
        return primes.length() > 0 ? primes.substring(0, primes.length() - 1) : "No primes found";
    }

    private boolean isPrime(int num) {
        if (num < 2) return false;
        for (int i = 2; i * i <= num; i++) {
            if (num % i == 0) return false;
        }
        return true;
    }
}
