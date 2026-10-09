#include <stdlib.h>
#include <string.h>

#include "../include/helpers.h"

#include <stdio.h>

char *dup_string(const char *string) {
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

size_t hash(const char *key) {
    size_t hash = 5381;
    size_t len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        hash = hash * 33 + (unsigned char) key[i];
    }
    return hash;
}

Entry *find_entry(const KVStore *kvs, const char*key) {
    const size_t bucket_index = hash(key) % kvs->capacity;
    Entry *chain = (kvs->buckets)[bucket_index];
    while (chain != NULL && strcmp(chain->key, key) != 0 ) {
        chain = chain->entry;
    }
    return chain;
}


int is_first_in_chain(KVStore *kvs, Entry *e) {
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

        if ( hash1 == hash2 && strcmp(kvs->buckets[i]->key, e->key) == 0) {
            return 0;
        }
    }
    return 1;
}

Entry *find_entry_superlink(KVStore *kvs, Entry *e) {
    const size_t bucket_index = hash(e->key) % kvs->capacity;
    Entry *chain = (kvs->buckets)[bucket_index];
    if (e == chain) {
        return chain;
    }
    while (chain != NULL && strcmp(chain->entry->key, e->key) != 0 ) {
        chain = chain->entry;
    }
    return chain;
}

size_t kv_count(const KVStore *kvs) {
    if (kvs == NULL) {
        return 0;
    }
    return kvs->count;
}