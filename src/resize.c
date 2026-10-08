#include <stdio.h>

#include "../include/helpers.h"
#include "../include/resize.h"

#include <stdlib.h>

int needs_resize(const KVStore *kvs) {

    const size_t threshold = ((kvs->count)+1) * 4 / kvs->capacity * 3;
    printf("\nThreshold: %lu", threshold);
    if (threshold > 75) {
        return 1;
    }
    return 0;
}

void table_resize(KVStore *kvs) {
    const size_t new_capacity = kvs->capacity * 2;
    Entry **new_buckets = calloc(new_capacity, sizeof(Entry *));

    if (new_buckets == NULL) {
        printf("\nERROR: failed to allocate bucket");
        return;
    }

    for (size_t i = 0; i < kvs->capacity; i++) {
        Entry *chain = kvs->buckets[i];

        while (chain != NULL) {
            Entry *chain_next = chain->entry;
            chain->entry = NULL;

            const size_t bucket_index = hash(chain->key) % new_capacity;

            if (new_buckets[bucket_index] == NULL) {
                new_buckets[bucket_index] = chain;
            } else {
                Entry *n_bckt_chain = new_buckets[bucket_index];

                while (n_bckt_chain->entry != NULL) {
                    n_bckt_chain = n_bckt_chain->entry;
                }

                n_bckt_chain->entry = chain;
            }

            chain = chain_next;
        }
    }

    kvs->capacity = kvs->capacity * 2;
    free(kvs->buckets);
    kvs->buckets = new_buckets;
}
