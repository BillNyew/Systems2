import java.io.*;
import java.net.*;
import java.util.*;

public class Head {
    private static final int PORT = 5000;
    private static final int NODE_COUNT = 3; // Change as needed
    private List<Socket> nodes = new ArrayList<>();
    private int rangeStart = 1000;
    private int rangeEnd = 1000000;

    public static void main(String[] args) {
        new Head().startServer();
    }

    public void startServer() {
        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Head started, waiting for nodes...");

            while (nodes.size() < NODE_COUNT) {
                Socket nodeSocket = serverSocket.accept();
                nodes.add(nodeSocket);
                System.out.println("Node connected: " + nodeSocket.getInetAddress());
            }
            System.out.println("All nodes connected. Assigning work...");
            assignWork();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void assignWork() {
        int rangeSize = (rangeEnd - rangeStart) / NODE_COUNT;
        List<Thread> threads = new ArrayList<>();

        for (int i = 0; i < NODE_COUNT; i++) {
            int start = rangeStart + (i * rangeSize);
            int end = (i == NODE_COUNT - 1) ? rangeEnd : start + rangeSize - 1;

            final int nodeIndex = i;
            Thread thread = new Thread(() -> handleNode(nodes.get(nodeIndex), start, end));
            threads.add(thread);
            thread.start();
        }

        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println("All results received. Computation complete.");
    }

    private void handleNode(Socket node, int start, int end) {
    
      try {
          PrintWriter out = new PrintWriter(node.getOutputStream(), true);
          BufferedReader in = new BufferedReader(new InputStreamReader(node.getInputStream()));
  
          System.out.println("Assigning work to " + node.getInetAddress() + ": " + start + " to " + end);
          out.println(start + " " + end);  // Send the range
  
          String result = in.readLine();
          System.out.println("Received from node " + node.getInetAddress() + ": " + result);
  
          //node.close();
      } catch (IOException e) {
          e.printStackTrace();
      }
    }
}
