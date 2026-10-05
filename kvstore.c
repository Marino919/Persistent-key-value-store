#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

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

static char *dup_string(const char *string) {
    const size_t len = strlen(string);
    char *retr_string = malloc((len+1)*sizeof(char));
    if (retr_string == NULL) {
        return NULL;
    }
    for (size_t i =0; i < len+1; i++) {
        retr_string[i] = string[i];
    }
    return retr_string;
}

static size_t hash(const char *key) {
    size_t hash = 5381;
    size_t len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        hash = hash * 33 + (unsigned char) key[i];
    }
    return hash;
}

static Entry *find_entry(const KVStore *kvs, const char*key) {
    const size_t bucket_index = hash(key) % kvs->capacity;
    Entry *chain = (kvs->buckets)[bucket_index];
    while (chain != NULL && strcmp(chain->key, key) != 0 ) {
        chain = chain->entry;
    }
    return chain;
}

KVStore *kv_open( const char *path ) {
    KVStore *kvs = calloc(1, sizeof(KVStore));
    if (kvs == NULL) {
        free(kvs);
        return NULL;
    }
    kvs->capacity = INITIAL_CAPACITY;
    kvs->count = 0;
    kvs->buckets = calloc(INITIAL_CAPACITY, sizeof(Entry *));
    if (kvs->buckets == NULL) {
        free(kvs->buckets);
        free(kvs);
        return NULL;
    }

    return kvs;
}

static int table_set(KVStore *kvs, const char *key, const char *value) {

    char *new_value = dup_string(value);
    if (new_value == NULL) {
        free(new_value);
        return -1;
    }

    Entry *existing = find_entry(kvs, key);
    if (existing != NULL) {
        free(existing->value);
        existing->value = new_value;
        return 0;
    }

    char *new_key = dup_string(key);
    if (new_key == NULL) {
        free(new_key);
        return -1;
    }

    Entry *new_entry = calloc(1, sizeof(Entry));
    if (new_entry == NULL) {
        free(new_entry);
        return -1;
    }
    new_entry->key = new_key;
    new_entry->value = new_value;

    const size_t bucket_index = hash(key) % kvs->capacity;
    Entry **chain = &(kvs->buckets)[bucket_index];

    if (*chain == NULL) {
        *chain = new_entry;
    } else {
        while ((*chain)->entry!=NULL) {
            *chain = (*chain)->entry;
        }
        (*chain)->entry = new_entry;
    }
    kvs->count++;
    return 1;
}
static void close_entry(Entry *entry) {
    free(entry->key);
    free(entry->value);
    free(entry->entry);
    free(entry);
}
static int table_remove(KVStore *kvs, const char *key) {
    Entry *delete_entry = find_entry(kvs, key);
    if (delete_entry == NULL) {
        return 1;
    }
    Entry *rebase_entry = delete_entry->entry;
    if (rebase_entry == NULL) {
        free(delete_entry);
        return 0;
    }
    free(delete_entry->key);
    delete_entry->key = rebase_entry->key;
    free(delete_entry->value);
    delete_entry->value = rebase_entry->value;
    free(delete_entry->entry);
    delete_entry->entry = rebase_entry->entry;

    close_entry(rebase_entry);

    return 0;
}
static void close_chain(Entry *chain) {
    if (chain == NULL) return;

    if (chain->entry != NULL) {
        close_chain(chain->entry);
    }
    close_entry(chain);
}

void kv_close( KVStore *kvs) {
    for (size_t i = 0; i < kvs->capacity; i++) {
        close_chain((kvs->buckets)[i]);
    }
    free(kvs->buckets);
    free(kvs);
}





