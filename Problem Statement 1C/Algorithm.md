# LRU Cache for File Information

## Objective
Implement a Least Recently Used (LRU) Cache that:
- Stores information about files (file path, open timestamp, metadata such as size, inode, etc.).
- Removes the least recently used files when the cache reaches capacity or files are too old.
- Supports creating the cache, adding new entries, searching entries by file path, and removing old entries.

---

## Data Structures

1. **`LRUNode`** – Represents each file entry in the cache.
   - `char filepath[MAX_PATH_LEN]` – Stores the full path of the file.
   - `time_t open_time` – Timestamp when the file was added.
   - `struct stat metadata` – Stores file metadata using `stat()`.
   - `struct LRUNode* prev` – Points to the previous node.
   - `struct LRUNode* next` – Points to the next node.

2. **`LRUCache`** – Represents the cache structure.
   - `LRUNode* head` – Points to the most recently used file.
   - `LRUNode* tail` – Points to the least recently used file.
   - `int size` – Current number of elements in the cache.
   - `int capacity` – Maximum allowed elements in the cache.

---

## Algorithm Steps

### 1. Create LRU Cache

**Function**: `LRUCache* create_lru_cache(int capacity)`

- Allocate memory for an `LRUCache` structure.
- Initialize:
  - `head` and `tail` as `NULL`.
  - `size` as `0`.
  - `capacity` with the given value.
- Return the created `LRUCache` pointer.

---

### 2. Add Entry to LRU Cache

**Function**: `void add_to_lru(LRUCache* cache, const char* filepath)`

- Call `search_lru(cache, filepath)`:
  - If file exists, it is already moved to the front; return.
- Create a new `LRUNode`:
  - Copy the `filepath` using `strncpy`.
  - Set `open_time` to current time using `time(NULL)`.
  - Fill `metadata` using `stat(filepath, &new_node->metadata)`.
- Insert node at the front of the list:
  - Set `new_node->next = cache->head`
  - If `cache->head` exists, update its `prev = new_node`
  - Set `cache->head = new_node`
  - If `cache->tail == NULL`, set `cache->tail = new_node`
- Increment `cache->size`.
- If `size > capacity`, call `remove_lru_tail(cache)` to evict the least recently used file.

---

### 3. Search Entry by File Path

**Function**: `LRUNode* search_lru(LRUCache* cache, const char* filepath)`

- Start from `cache->head`, iterate through the list.
- Compare each node’s `filepath` with the given path.
- If found:
  - Call `move_to_front(cache, node)`
  - Return the node pointer.
- If not found, return `NULL`.

---

### 4. Move Node to Front

**Function**: `void move_to_front(LRUCache* cache, LRUNode* node)`

- If node is already the head, do nothing.
- Re-link surrounding nodes to remove `node`:
  - If `node->prev`, set `node->prev->next = node->next`
  - If `node->next`, set `node->next->prev = node->prev`
- If node is the tail, update `cache->tail = node->prev`
- Insert node at front:
  - `node->prev = NULL`
  - `node->next = cache->head`
  - If `cache->head`, set `cache->head->prev = node`
  - Update `cache->head = node`

---

### 5. Remove Least Recently Used Entry

**Function**: `void remove_lru_tail(LRUCache* cache)`

- If `cache->tail == NULL`, return.
- Store pointer to `cache->tail` in a temporary variable.
- Update `cache->tail = node->prev`
- If `cache->tail` is not `NULL`, set `cache->tail->next = NULL`
- If removed node is also the head, set `cache->head = NULL`
- Free the memory of the removed node.
- Decrement `cache->size`.

---

## Example Execution

**Cache Capacity = 3**

- Initial Cache:
