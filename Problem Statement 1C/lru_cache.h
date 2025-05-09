#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <time.h>
#include <sys/stat.h>

#define MAX_PATH_LEN 1024

typedef struct LRUNode {
    char filepath[MAX_PATH_LEN];
    time_t open_time;
    struct stat metadata;
    struct LRUNode* prev;
    struct LRUNode* next;
} LRUNode;

typedef struct {
    LRUNode* head;
    LRUNode* tail;
    int capacity;
    int size;
    time_t timeout_sec;
} LRUCache;

LRUCache* create_lru_cache(int capacity, time_t timeout_sec);
void free_lru_cache(LRUCache* cache);
void add_to_lru(LRUCache* cache, const char* filepath);
LRUNode* search_lru(LRUCache* cache, const char* filepath);
void remove_old_entries(LRUCache* cache);
void remove_entry(LRUCache* cache, const char* filepath);

#endif
