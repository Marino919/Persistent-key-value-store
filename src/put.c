#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/helpers.h"
#include "../include/resize.h"
#include "../include/put.h"

static int table_set(KVStore *kvs, const char *key, const char *value) {

    char *new_value = dup_string(value);
    if (new_value == NULL) {
        free(new_value);
        printf("\nERROR: dup string of value is NULL");
        return -1;
    }

    Entry *existing = find_entry(kvs, key);
    if (existing != NULL) {//already existing key
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

    if ( needs_resize(kvs) ) {
        printf("\nResizing table...");
        table_resize(kvs);
        printf("done!");
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

int kv_put( KVStore *kvs, const char *key, const char *value) {
    int success = table_set(kvs, key, value);
    return success;
}