#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "door.h"
#include "get.h"
#include "helpers.h"
#include "print_store.h"
#include "put.h"
#include "remove.h"
#include "optimize_log.h"

int process_command(KVStore *kvs, char buffer[1024]) {

    if (kvs == NULL) {
        return 0;
    }

    char full_command[1025];
    snprintf(full_command, 1024, "%s\n", buffer);
    char *op = strtok(buffer, " \t");
    uint8_t operation = 0;
    if (strcmp(op, "put") == 0) {
        operation = OP_PUT;
    } else if (strcmp(op, "delete") == 0){
        operation = OP_DELETE;
    }

    if (operation == OP_DELETE) {
        char *key = strtok(NULL, " \t\n");
        if (key == NULL || strlen(key) > MAX_KEY_LEN) {
            printf("-> Key improperly specified\n");
            return 0;
        }
        //printf("-> Deleting key %s", key);
        int success = kv_delete(kvs, key);
        if (success == 0) {
            printf("-> key [%s] deleted\n", key);
            return 0;
        }

        printf("-> key not found or an error happened\n");
        return -1;
    }


    if (operation == OP_PUT) {
        char *key = strtok(NULL, " \t\n");
        char *value = strtok(NULL, "\n");
        if (key == NULL || strlen(key) > MAX_KEY_LEN) {
            printf("-> Key improperly specified\n");
            return 0;
        }
        if (value == NULL || strlen(value) > MAX_VALUE_LEN) {
            printf("-> Value improperly specified\n");
            return 0;
        }
        int success = kv_put(kvs, key, value);
        //printf("-> Putting value %s in %s", value, key);
        if (success == 0) {
            printf("-> Putting successfull\n");
            return 0;
        }
        printf("->Putting is NOT successfull\n");
        return -1;
    }


    if (strcmp(op, "get") == 0) {
        char *key = strtok(NULL, " \t\n");
        if (key == NULL || strlen(key) > MAX_KEY_LEN) {
            printf("-> Key improperly specified\n");
            return 0;
        }
        const char *value = kv_get(kvs, key);
        if (value == NULL) {
            printf("ERROR: key doesnt exist\n");
            return -1;
        }
        printf("ENTRY: [%s]-{%s}\n", key, value);
        return 0;
    }

    if (strcmp(op, "pstore") == 0) {
        kv_print_store(kvs);
        return 0;
    }

    if (strcmp(op, "close") == 0) {
        kv_print_store(kvs);
        kv_close(kvs);
        return -1;
    }

    if (strcmp(op, "optlog") == 0) {
        int success = optimize_log(kvs);
        if (success == 0) {
            printf("Log successfuly optimized!\n");
            return 0;
        }
        return -1;
    }

    printf("That is not a command!\n");
    return 0;

}
