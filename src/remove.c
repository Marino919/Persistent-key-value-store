#include "remove.h"
#include "helpers.h"
#include "log_command.h"

//returns -1 or 0
int table_remove(KVStore *kvs, const char *key) {
    Entry *delete_entry = find_entry(kvs, key);
    if (delete_entry == NULL) {
        //printf("\nWARNING: key not found");
        return 0;
    }

    const size_t bucket_index = hash(key) % kvs->capacity;

    if (is_first_in_chain(kvs, delete_entry) == 0 && delete_entry->entry == NULL) {
        free(delete_entry->key);
        free(delete_entry->value);
        free(delete_entry);
        kvs->buckets[bucket_index] = NULL;
        kvs->count--;
        return 0;
    }

    Entry *rebase_entry = delete_entry->entry;
    if (is_first_in_chain(kvs, delete_entry) == 0 && delete_entry->entry != NULL) {
        free(delete_entry->key);
        free(delete_entry->value);
        free(delete_entry);
        kvs->buckets[bucket_index] = rebase_entry;
        kvs->count--;
        return 0;
    }

    if (is_first_in_chain(kvs, delete_entry) != 0 && rebase_entry != NULL) {
        free(delete_entry->key);
        free(delete_entry->value);
        delete_entry->key = rebase_entry->key;
        delete_entry->value = rebase_entry->value;
        delete_entry->entry = rebase_entry->entry;
        free(rebase_entry);
        kvs->count--;
        return 0;
    }

    if (is_first_in_chain(kvs, delete_entry) != 0 && rebase_entry == NULL) {
        Entry *superlink = find_entry_superlink(kvs, delete_entry);
        free(delete_entry->key);
        free(delete_entry->value);
        free(superlink->entry);
        superlink->entry = NULL;
        kvs->count--;
        return 0;
    }

    printf("\nERROR: something went wrong while deleting this entry: %s", delete_entry->key);
    return -1;
}

int kv_delete( KVStore *kvs, const char *key) {
    Entry *e = find_entry(kvs, key);
    if ( e == NULL) return -1;
    int success = log_command(kvs->log, OP_DELETE, key, NULL);
    if (success < 0) return -1;
    success = table_remove(kvs, key);
    return success;//-1 or 0
}