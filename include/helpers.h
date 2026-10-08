#ifndef KVSTORE_HELPERS_H
#define KVSTORE_HELPERS_H

#define INITIAL_CAPACITY 2

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *entry;
} Entry;

typedef struct KVStore {
    Entry **buckets;
    size_t capacity;
    size_t count;
} KVStore;


char *dup_string(const char *string);
size_t hash(const char *key);
Entry *find_entry(const KVStore *kvs, const char*key);
Entry *find_entry_superlink(KVStore *kvs, Entry *e);
int is_first_in_chain(KVStore *kvs, Entry *e);
size_t kv_count(const KVStore *kvs);

#endif //KVSTORE_HELPERS_H