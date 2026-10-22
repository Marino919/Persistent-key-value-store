#include <stdlib.h>

#include "../include/helpers.h"
#include "../include/door.h"

#include <string.h>

#include "command_processor.h"
#include "endian.h"
#include "helpers.h"
#include "put.h"
#include "remove.h"

static int read_record(KVStore *kvs, uint8_t data[9]) {
    uint8_t op = data[0];
    uint32_t key_len32 = get_u32_le(&data[1]);
    uint32_t value_len32 = get_u32_le(&data[5]);
    char *key = NULL;
    char *value = NULL;
    int ok = 1;

    if (op != 1 && op != 2) goto done;
    if (key_len32 > MAX_KEY_LEN || value_len32 > MAX_VALUE_LEN) goto done;
    if (op == 2 && value_len32 != 0) goto done;

    key = malloc((size_t)key_len32 + 1);
    if (key == NULL) goto done;

    size_t key_len = fread(key, 1, key_len32, kvs->log);
    if (key_len != key_len32) goto done;
    key[key_len32] = '\0';

    if (op == 1) {

        value = malloc((size_t)value_len32 + 1);
        if (value == NULL) goto done;
        size_t val_len = fread(value, 1, value_len32, kvs->log);
        if (value_len32 != val_len) goto done;
        value[value_len32] = '\0';

        int success = kv_put(kvs, key, value);
        if ( success != 0) goto done;

    } else if (op == 2 && value_len32 == 0) {
        int success = kv_delete(kvs, key);
        if ( success != 0) goto done;
    }

    ok = 0;

    done:
        free(key);
        free(value);
        return ok;
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
    if (kvs->buckets == NULL) goto err;
    kvs->path = dup_string(path);
    if (kvs->path == NULL) goto err;
    kvs->log = fopen(kvs->path, "rb");
    if (kvs->log != NULL) {

        printf("\nLoading store...");
        uint8_t data[9] = {0};
        size_t num_data = fread(data, sizeof(uint8_t), 9, kvs->log);
        while (num_data != 0 && num_data == 9) {
            int success = read_record(kvs, data);
            if (success != 0) goto err;
            num_data = fread(data, sizeof(uint8_t), 9, kvs->log);
        }

        if (num_data != 0) goto err;
        fclose(kvs->log);
        kvs->log = NULL;

    } else {
        printf("Creating new store...\n");
    }

    kvs->log = fopen(kvs->path, "ab");
    if (kvs->log == NULL) goto err;
    printf("\nLog open, ready to write! \n");

    return kvs;
    err:
        kv_close(kvs);
        return NULL;
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
    if (kvs == NULL) {
        return;
    }
    if (kvs->buckets != NULL) {
        for (size_t i = 0; i < kvs->capacity; i++) {
            close_chain((kvs->buckets)[i]);
        }
    }

    free(kvs->path);
    free(kvs->buckets);
    if (kvs->log != NULL ) {
        fclose(kvs->log);
    }
    free(kvs);
    kvs = NULL;
}

