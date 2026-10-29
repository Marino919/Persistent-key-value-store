#ifndef KVSTORE_HELPERS_H
#define KVSTORE_HELPERS_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INITIAL_CAPACITY 2
#define MAX_VALUE_LEN 1024
#define MAX_KEY_LEN 1024
#define OP_PUT 1
#define OP_DELETE 2
#define HEADER_N 13
#define REC_OK 0
#define REC_TORN 1
#define REC_CORRUPT 2

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *entry;
} Entry;

typedef struct KVStore {
    Entry **buckets;
    size_t capacity;
    size_t count;
    FILE *log;
    char *path;
} KVStore;


char *dup_string(const char *string);
size_t hash(const char *key);
Entry *find_entry(const KVStore *kvs, const char*key);
Entry *find_entry_superlink(KVStore *kvs, Entry *e);
int is_first_in_chain(KVStore *kvs, Entry *e);
size_t kv_count(const KVStore *kvs);

#endif //KVSTORE_HELPERS_H