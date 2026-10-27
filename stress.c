#include <stdlib.h>

#include "optimize_log.h"
#include "include/kv_basic.h"
#define NKEYS 500

typedef struct {
    int present;
    char value[256];
} ModelSlot;

ModelSlot model[NKEYS] = {0};

int checkValue(KVStore *kvs, int key) {
    char key_store[16];
    snprintf(key_store, sizeof key_store, "key%d", key);
    if (model[key].present == 1) {
        char *value_store = kv_get(kvs, key_store);
        if (value_store == NULL) return 1;
        if (strcmp(value_store, model[key].value) != 0) return 1;
    } else {
        char *value_store = kv_get(kvs, key_store);
        if (value_store != NULL) return 1;
    }
    return 0;
}

int checkValues(KVStore *kvs) {
    for (int k = 0; k < NKEYS; k++) {

        int success = checkValue(kvs, k);
        if (success == 1) {
            return 1;
        }
    }
    return 0;
}


int main(int argc, char *argv[]) {
    unsigned seed = 1;

    remove("stress.bin");
    KVStore *kvs= kv_open("stress.bin");
    if (kvs == NULL) {
        printf("\nstore opening failed");
        exit(1);
    }
    for (int x = 0; x < 200; x++) {
        srand(seed);

        for (int j = 0; j < 200; j ++) {
        int roll = rand() % 100;
        int i = rand() % NKEYS;
        char key[16];
        snprintf(key, sizeof key, "key%d", i);
        if (roll < 45) {
            int len_value = 1 + rand() % 100;
            char value[len_value+1];
            value[len_value] = '\0';
            for (int m = 0; m < len_value; m++) {
                char character = 'a' + rand() % 26;
                value[m] = character;
            }

            int success = kv_put(kvs, key, value);

            if (success < 0) {
                fprintf(stderr, "\nput, seed: %u, iteration: %d, key: %s", seed, j, key);
                exit(1);
            }

            model[i].present = 1;
            snprintf(model[i].value, len_value+1, "%s", value);
        } else if (roll < 65) {
            int expected = model[i].present ? 0 : -1;
            int got = kv_delete(kvs, key);
            if (got != expected) {
                fprintf(stderr, "delete: seed %u, iteration %d, key %s: expected %d, got %d\n",
                        seed, j, key, expected, got);
                exit(1);
            }
            model[i].present = 0;
            model[i].value[0] = '\0';

        } else if (roll < 90) {
            int success = checkValue(kvs, i);
            if (success == 1) {
                fprintf(stderr,"\nget failed, seed: %u, iteration: %d, key: %s", seed, j, key);
                exit(1);
            }
        } else if (roll < 97) {
            kv_close(kvs);
            kvs = kv_open("stress.bin");
            if (kvs == NULL) {
                fprintf(stderr, "\n open failed, seed: %u, iteration: %d, key: %s", seed, j, key);
                exit(1);
            }
            int fail = checkValues(kvs);
            if (fail == 1) {
                fprintf(stderr, "\n check failed, seed: %u, iteration: %d, key: %s", seed, j, key);
                exit(1);
            }
        } else {
            int success = optimize_log(kvs);
            int fail = checkValues(kvs);
            if (fail == 1 || success == 1) {
                fprintf(stderr, "\noptimisation fail, seed: %u, iteration: %d, key: %s", seed, j, key);
                exit(1);
            }
        }
    }
    }
    kv_print_store(kvs);
    kv_close(kvs);
    return 0;
}