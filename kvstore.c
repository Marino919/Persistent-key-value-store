#include <stddef.h>
#include <stdio.h>
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
        printf("\nERROR: dup string of value is NULL");
        return -1;
    }

    Entry *existing = find_entry(kvs, key);
    if (existing != NULL) {
        printf("\nWARNING: key already exists, the value has been updated");
        free(existing->value);
        existing->value = new_value;
        return 0;
    }

    char *new_key = dup_string(key);
    if (new_key == NULL) {
        printf("\nERROR: dup string of key is NULL");
        free(new_key);
        return -1;
    }

    Entry *new_entry = calloc(1, sizeof(Entry));
    if (new_entry == NULL) {
        printf("\nERROR: new entry failed to generate");
        free(new_entry);
        return -1;
    }
    new_entry->key = new_key;
    new_entry->value = new_value;

    const size_t bucket_index = hash(key) % kvs->capacity;

    if (kvs->buckets[bucket_index] == NULL) {
        kvs->buckets[bucket_index] = new_entry;
    } else {
        Entry *chain = (kvs->buckets)[bucket_index];
        while (chain->entry != NULL) {
            chain = chain->entry;
        }
        chain->entry = new_entry;
    }

    kvs->count++;
    return 0;
}

static int is_first_in_chain(KVStore *kvs, Entry *e) {
    if (e == NULL || kvs == NULL) {
        printf("\nERROR: entry or KVStore doesnt exist");
        return 1;
    }
    for (size_t i = 0; i < kvs->capacity; i++) {
        if (kvs->buckets[i] == NULL) {
            continue;
        }
        size_t hash1 = hash(kvs->buckets[i]->key);
        size_t hash2 = hash(e->key);

        if ( hash1 == hash2) {
            return 0;
        }
    }
    return 1;
}

static Entry *find_entry_superlink(KVStore *kvs, Entry *e) {
    const size_t bucket_index = hash(e->key) % kvs->capacity;
    Entry *chain = (kvs->buckets)[bucket_index];
    while (chain != NULL && strcmp(chain->entry->key, e->key) != 0 ) {
        chain = chain->entry;
    }
    return chain;
}

static int table_remove(KVStore *kvs, const char *key) {
    Entry *delete_entry = find_entry(kvs, key);
    if (delete_entry == NULL) {
        printf("\n ERROR: key not found");
        return 1;
    }

    if (delete_entry->entry == NULL) {
        Entry *e = find_entry_superlink(kvs, delete_entry);
        e->entry = NULL;
        free(delete_entry->key);
        free(delete_entry->value);
        free(delete_entry);
        delete_entry = NULL;
        return 0;
    }

    const size_t bucket_index = hash(key) % kvs->capacity;

    if (is_first_in_chain(kvs, delete_entry) == 0 && delete_entry->entry == NULL) {
        free(delete_entry->key);
        free(delete_entry->value);
        free(delete_entry);
        kvs->buckets[bucket_index] = NULL;
        return 0;
    }

    Entry *rebase_entry = delete_entry->entry;
    if (is_first_in_chain(kvs, delete_entry) == 0 && delete_entry->entry != NULL) {
        free(delete_entry->key);
        free(delete_entry->value);
        free(delete_entry);
        kvs->buckets[bucket_index] = rebase_entry;
        return 0;
    }

    if (is_first_in_chain(kvs, delete_entry) != 0) {
        free(delete_entry->key);
        free(delete_entry->value);
        delete_entry->key = rebase_entry->key;
        delete_entry->value = rebase_entry->value;
        delete_entry->entry = rebase_entry->entry;

        return 0;
    }

    printf("ERROR: something went wrong while deleting this entry: %s", delete_entry->key);


    kvs->count--;
    return 0;
}
static void close_chain(Entry *chain) {
    if (chain == NULL) return;

    if (chain->entry != NULL) {
        close_chain(chain->entry);
    }
    free(chain->key);
    free(chain->value);
    free(chain);
}

void kv_close( KVStore *kvs) {
    for (size_t i = 0; i < kvs->capacity; i++) {
        close_chain((kvs->buckets)[i]);
    }
    free(kvs->buckets);
    kvs = NULL;
    free(kvs);
}

int kv_put( KVStore *kvs, const char *key, const char *value) {
    int success = table_set(kvs, key, value);
    return success;
}

const char *kv_get(KVStore *kvs, const char *key) {
    Entry *fe = find_entry(kvs, key);
    if (fe == NULL) {
        return NULL;
    }
    return fe->value;
}

int kv_delete( KVStore *kvs, const char *key) {
    int success = table_remove(kvs, key);
    return success;
}

size_t kv_count(const KVStore *kvs) {
    if (kvs == NULL) {
        return 0;
    }
    return kvs->count;
}

void kv_print_store(KVStore *kvs) {
    if (kvs == NULL) {
        printf("ERROR: Unable to print store");
        return;
    }
    printf("Printing current store: ");
    for (size_t i = 0; i < kvs->capacity; i++) {
        printf("\n>");
        Entry *ent = (kvs->buckets)[i];
        while (ent != NULL) {
            printf("[%s]-{%s}  ", ent->key, ent->value);
            ent = ent->entry;
        }

    }
    printf("\n");
}


