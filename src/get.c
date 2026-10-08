#include <stddef.h>

#include "../include/helpers.h"

const char *kv_get(KVStore *kvs, const char *key) {
    Entry *fe = find_entry(kvs, key);
    if (fe == NULL) {
        return NULL;
    }
    return fe->value;
}
