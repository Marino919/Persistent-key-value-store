
#include <stddef.h>
#include <stdio.h>

#include "../include/helpers.h"
void kv_print_store(KVStore *kvs) {
    if (kvs == NULL) {
        printf("\nERROR: Unable to print store");
        return;
    }
    printf("\nPrinting current store: ");
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
