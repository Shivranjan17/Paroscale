#include "lru_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void move_to_head(LRUCache* cache, LRUNode* node) {
    if (cache->head == node) return;

    // Detach node
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (cache->tail == node) cache->tail = node->prev;

    // Move to head
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

static void remove_tail(LRUCache* cache) {
    if (!cache->tail) return;
    LRUNode* to_remove = cache->tail;

    if (to_remove->prev) {
        to_remove->prev->next = NULL;
        cache->tail = to_remove->prev;
    } else {
        cache->head = cache->tail = NULL;
    }

    free(to_remove);
    cache->size--;
}

LRUCache* create_lru_cache(int capacity, time_t timeout_sec) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->head = cache->tail = NULL;
    cache->capacity = capacity;
    cache->size = 0;
    cache->timeout_sec = timeout_sec;
    return cache;
}

void free_lru_cache(LRUCache* cache) {
    LRUNode* current = cache->head;
    while (current) {
        LRUNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(cache);
}

void add_to_lru(LRUCache* cache, const char* filepath) {
    LRUNode* existing = search_lru(cache, filepath);
    if (existing) {
        existing->open_time = time(NULL);
        stat(filepath, &existing->metadata);
        move_to_head(cache, existing);
        return;
    }

    if (cache->size >= cache->capacity) {
        remove_tail(cache);
    }

    LRUNode* new_node = (LRUNode*)malloc(sizeof(LRUNode));
    strncpy(new_node->filepath, filepath, MAX_PATH_LEN);
    new_node->open_time = time(NULL);
    stat(filepath, &new_node->metadata);
    new_node->prev = NULL;
    new_node->next = cache->head;

    if (cache->head) cache->head->prev = new_node;
    cache->head = new_node;
    if (!cache->tail) cache->tail = new_node;
    cache->size++;
}

LRUNode* search_lru(LRUCache* cache, const char* filepath) {
    LRUNode* current = cache->head;
    while (current) {
        if (strcmp(current->filepath, filepath) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void remove_old_entries(LRUCache* cache) {
    time_t now = time(NULL);
    LRUNode* current = cache->tail;

    while (current) {
        LRUNode* prev = current->prev;
        if (difftime(now, current->open_time) > cache->timeout_sec) {
            if (current->prev) current->prev->next = current->next;
            if (current->next) current->next->prev = current->prev;
            if (cache->head == current) cache->head = current->next;
            if (cache->tail == current) cache->tail = current->prev;

            free(current);
            cache->size--;
        }
        current = prev;
    }
}

void remove_entry(LRUCache* cache, const char* filepath) {
    LRUNode* node = search_lru(cache, filepath);
    if (!node) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (cache->head == node) cache->head = node->next;
    if (cache->tail == node) cache->tail = node->prev;

    free(node);
    cache->size--;
}
int main() {
    LRUCache* cache = create_lru_cache(3, 5); // capacity 3, timeout 5 seconds

    add_to_lru(cache, "/tmp/file1.txt");
    sleep(1);
    add_to_lru(cache, "/tmp/file2.txt");
    sleep(1);
    add_to_lru(cache, "/tmp/file3.txt");
    sleep(1);
    add_to_lru(cache, "/tmp/file4.txt"); // Should evict file1

    LRUNode* result = search_lru(cache, "/tmp/file1.txt");
    if (result) {
        printf("File1 found: %s\n", result->filepath);
        printf("Open time: %s", ctime(&result->open_time));
        printf("Size: %ld bytes\n", result->metadata.st_size);
        printf("Inode: %ld\n", result->metadata.st_ino);
        printf("Last Modified: %s", ctime(&result->metadata.st_mtime));
    } else {
        printf("File1 not found or evicted\n");
    }
    

    remove_old_entries(cache);

    free_lru_cache(cache);
    return 0;
}