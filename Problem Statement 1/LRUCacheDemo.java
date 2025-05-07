import java.io.IOException;

public class LRUCacheDemo {
    public static void main(String[] args) throws IOException, InterruptedException {
        LRUCache cache = new LRUCache(3, 10); 

        
        cache.addEntry("/Users/shivranjanpathak/file1.txt");
        cache.addEntry("/Users/shivranjanpathak/file2.txt");
        cache.addEntry("/Users/shivranjanpathak/file3.txt");

       
        System.out.println("Cache after adding 3 files:");
        cache.printCache();

        cache.getEntry("/Users/shivranjanpathak/file1.txt");

       
        cache.addEntry("/Users/shivranjanpathak/file4.txt");

        System.out.println("\nCache after adding file4:");
        cache.printCache();


        Thread.sleep(11000);

        
        cache.removeOldEntries();

        System.out.println("\nCache after removing expired entries:");
        cache.printCache();
    }
}
