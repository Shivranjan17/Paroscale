import java.io.*;
import java.util.*;
import java.util.concurrent.*;

public class UniqueNumberExtractor {

    // Thread-safe global set
    private static final Set<Integer> globalUniqueSet = ConcurrentHashMap.newKeySet();

    // Worker class to process part of the list
    static class Worker implements Runnable {
        private final List<Integer> data;

        public Worker(List<Integer> data) {
            this.data = data;
        }

        @Override
        public void run() {
            for (int num : data) {
                globalUniqueSet.add(num);
            }
        }
    }

    public static void main(String[] args) throws Exception {
        File file = new File("abc.txt");
        List<Integer> numbers = new ArrayList<>();

        // Read entire file into a list
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                try {
                    numbers.add(Integer.parseInt(line.trim()));
                } catch (NumberFormatException ignored) {}
            }
        }

        // Divide list into parts for each thread
        int numThreads = 4;
        int chunkSize = numbers.size() / numThreads;
        List<Thread> threads = new ArrayList<>();

        for (int i = 0; i < numThreads; i++) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? numbers.size() : (i + 1) * chunkSize;
            List<Integer> sublist = numbers.subList(start, end);
            Thread thread = new Thread(new Worker(sublist));
            threads.add(thread);
            thread.start();
        }

        for (Thread t : threads) {
            t.join();
        }

        // Print result
        System.out.println("Unique Numbers:");
        globalUniqueSet.stream().sorted().forEach(System.out::println);
    }
}
