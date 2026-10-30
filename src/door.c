#include "door.h"
#include "helpers.h"
#include "crc32.h"
#include "endian.h"
#include "put.h"
#include "remove.h"

static int read_record(KVStore *kvs, uint8_t data[HEADER_N], long file_size) {
    uint32_t crc32_given = get_u32_le(&data[0]);
    uint8_t op = data[4 + 0];
    uint32_t key_len32 = get_u32_le(&data[4 + 1]);
    uint32_t value_len32 = get_u32_le(&data[4 + 5]);
    uint8_t *unprotected_data = NULL;
    char *key = NULL;
    char *value = NULL;
    int ok = REC_CORRUPT;

    if (op != 1 && op != 2) goto done;
    if (key_len32 > MAX_KEY_LEN || value_len32 > MAX_VALUE_LEN) goto done;
    if (op == 2 && value_len32 != 0) goto done;

    key = malloc((size_t)key_len32 + 1);
    if (key == NULL) goto done;

    ok = REC_TORN;

    size_t key_len = fread(key, 1, key_len32, kvs->log);
    if (key_len != key_len32) goto done;
    key[key_len32] = '\0';

    if (op == 1) {

        value = malloc((size_t)value_len32 + 1);
        if (value == NULL) goto done;
        size_t val_len = fread(value, 1, value_len32, kvs->log);
        if (value_len32 != val_len) goto done;
        value[value_len32] = '\0';

        size_t unpr_data_n = HEADER_N - 4 + key_len32 + value_len32;
        unprotected_data = calloc(unpr_data_n, sizeof(uint8_t));
        if (unprotected_data == NULL) goto done;
        memcpy(unprotected_data, &data[4 + 0], HEADER_N-4);
        memcpy(unprotected_data+HEADER_N-4, key, key_len32);
        memcpy(unprotected_data+HEADER_N-4+key_len32, value, value_len32);
        uint32_t calculated_crc32 = crc32(unprotected_data, unpr_data_n);
        long record_end = ftell(kvs->log);
        if (calculated_crc32 != crc32_given && record_end < file_size) {
            ok = REC_CORRUPT;
            goto done;
        }
        if (calculated_crc32 != crc32_given && record_end == file_size) goto done;

        int success = table_set(kvs, key, value);
        if ( success < 0) {
            ok = REC_CORRUPT;
            goto done;
        }


    } else if (op == 2 && value_len32 == 0) {

        size_t unpr_data_n = HEADER_N - 4 + key_len32;
        unprotected_data = calloc(unpr_data_n, sizeof(uint8_t));
        if (unprotected_data == NULL) goto done;
        memcpy(unprotected_data, &data[4 + 0], HEADER_N-4);
        memcpy(unprotected_data+HEADER_N-4, key, key_len32);
        uint32_t calculated_crc32 = crc32(unprotected_data, unpr_data_n);

        long record_end = ftell(kvs->log);
        if (calculated_crc32 != crc32_given && record_end < file_size) {
            ok = REC_CORRUPT;
            goto done;
        }
        if (calculated_crc32 != crc32_given && record_end == file_size) goto done;

        int success = table_remove(kvs, key);
        if ( success < 0) {
            ok = REC_CORRUPT;
            goto done;
        }
    }

    ok = REC_OK;

    done:
    free(unprotected_data);
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
    kvs->log = fopen(kvs->path, "rb+");
    uint8_t file_header[FILE_HEADER_N] = {0};
    int header_found = 0;

    if (kvs->log != NULL) {

        fseek(kvs->log, 0, SEEK_END);
        long file_size = ftell(kvs->log);
        if (file_size == -1) goto err;
        rewind(kvs->log);
        int torn = REC_OK;

        //Checking header
        if (file_size == 0) {
            write_file_header(kvs->log);
            fseek(kvs->log, 0, SEEK_END);
            file_size = ftell(kvs->log);
            if (file_size == -1) goto err;
            rewind(kvs->log);
        }
        int header_check = read_file_header(kvs->log);
        if (header_check == UNSUPPORTED_VERSION) goto err;
        if (header_check == NOT_A_STORE_FILE) goto err;
        if (header_check == -1) goto err;
        header_found = 1;

        //printf("Loading store...");
        long good_end = ftell(kvs->log);
        if (good_end == -1) goto err;
        uint8_t data[HEADER_N] = {0};
        size_t num_data = fread(data, sizeof(uint8_t), HEADER_N, kvs->log);
        while (num_data > 0 && !ferror(kvs->log)) { //&& num_data == HEADER_N
            int success = read_record(kvs, data, file_size);
            if ( num_data != HEADER_N ) success = REC_TORN;
            if (success == REC_CORRUPT) goto err;
            if (success == REC_TORN) {
                torn = REC_TORN;
                break;
            }
            good_end = ftell(kvs->log);
            num_data = fread(data, sizeof(uint8_t), HEADER_N, kvs->log);
        }

        if ( ferror(kvs->log) ) goto err;
        if (torn) {
            if (truncate(kvs->path, (off_t)good_end) != 0) goto err;
        }
        fclose(kvs->log);
        kvs->log = NULL;

    }

    if (header_found == 0) {
        kvs->log = fopen(kvs->path, "ab");
        if (kvs->log == NULL) goto err;

        write_file_header(kvs->log);

    } else {
        kvs->log = fopen(kvs->path, "ab");
        if (kvs->log == NULL) goto err;
    }


    //printf("\nLog open, ready to write! \n");

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

