import java.io.IOException;
import java.nio.file.*;
import java.nio.file.attribute.*;
import java.util.*;

class FileInfo {
    String filePath;
    long lastAccessedTime;
    long size;
    String inode;

    public FileInfo(String filePath, long lastAccessedTime, long size, String inode) {
        this.filePath = filePath;
        this.lastAccessedTime = lastAccessedTime;
        this.size = size;
        this.inode = inode;
    }
}

class LRUCache {
    private final int capacity;
    private final long expirationTimeSeconds; 
    private final LinkedHashMap<String, FileInfo> cache;

    public LRUCache(int capacity, long expirationTimeSeconds) {
        this.capacity = capacity;
        this.expirationTimeSeconds = expirationTimeSeconds;
        this.cache = new LinkedHashMap<>(capacity, 0.75f, true); 
    }

    public synchronized void addEntry(String filePath) throws IOException {
        Path path = Paths.get(filePath);
        BasicFileAttributes attrs = Files.readAttributes(path, BasicFileAttributes.class);

        long currentTime = System.currentTimeMillis() / 1000; 
        long size = attrs.size(); 
        String inode = attrs.fileKey().toString(); 

   
        cache.put(filePath, new FileInfo(filePath, currentTime, size, inode));
    }

    public synchronized FileInfo getEntry(String filePath) {
        return cache.get(filePath);
    }


    public synchronized void removeOldEntries() {
        long currentTime = System.currentTimeMillis() / 1000;
        Iterator<Map.Entry<String, FileInfo>> iterator = cache.entrySet().iterator();
        while (iterator.hasNext()) {
            Map.Entry<String, FileInfo> entry = iterator.next();
            
            if (currentTime - entry.getValue().lastAccessedTime > expirationTimeSeconds) {
                iterator.remove();
            }
        }
    }

    
    public synchronized void printCache() {
        for (Map.Entry<String, FileInfo> entry : cache.entrySet()) {
            FileInfo fileInfo = entry.getValue();
            System.out.println("Path: " + fileInfo.filePath + ", Size: " + fileInfo.size + " bytes, Inode: " + fileInfo.inode);
        }
    }
}