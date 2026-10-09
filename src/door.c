#include <stdlib.h>

#include "../include/helpers.h"
#include "../include/door.h"

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
    //kvs->path = dup_string(path);
    //kvs->log = fopen(kvs->path, "rb");
    return kvs;
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

