#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "door.h"
#include "endian.h"
#include "get.h"
#include "helpers.h"
#include "print_store.h"
#include "put.h"
#include "remove.h"
#include "optimize_log.h"

#define OP_PUT 1
#define OP_DELETE 2

static void log_command(KVStore *kvs, char *command) {

    char *op = strtok(command, " \t");
    char *key = strtok(NULL, " \t");
    char *value = strtok(NULL, " \t\n");
    uint8_t operation = 0;
    if (strcmp(op, "put") == 0) {
        operation = OP_PUT;
    } else {
        operation = OP_DELETE;
    }
    uint8_t key_len[4] = {0};
    uint8_t value_len[4] = {0};
    uint32_t key_len_32 = strlen(key);
    uint32_t value_len_32 = value ? strlen(value) : 0;
    put_u32_le(key_len, key_len_32);
    put_u32_le(value_len, value_len_32);

    uint8_t data[9] = {operation,
                        key_len[0], key_len[1], key_len[2], key_len[3],
                        value_len[0], value_len[1], value_len[2], value_len[3]};//only header

    fwrite(data, sizeof(uint8_t), 9, kvs->log);
    fwrite(key, sizeof(uint8_t), key_len_32, kvs->log);
    if (value_len_32 > 0) {
        fwrite(value, sizeof(uint8_t), value_len_32, kvs->log);
    }
    fflush(kvs->log);
}

int process_command(KVStore *kvs, char buffer[1024]) {
    if (kvs == NULL) {
        return 0;
    }

    char full_command[1025];
    snprintf(full_command, 1024, "%s\n", buffer);
    const char *delim = " \t\n";
    char *command = strtok(buffer, delim);
    if (command == NULL) {
        printf("ERROR: NULL command");
        return 0;
    }

    //printf("\nFULL COMMAND: %s", full_command);
    //printf("\ncommand given for executing: %s", command);

    if (strcmp(command, "delete") == 0) {
        char *key = strtok(NULL, delim);
        if (key == NULL) {
            printf("-> No key specified\n");
            return 0;
        }
        printf("-> Deleting key %s", key);
        int success = kv_delete(kvs, key);
        if (success == 0) {
            printf("\n-> key [%s] deleted\n", key);
            log_command(kvs, full_command);
            return 1;
        }
        printf("-> key not found or an error happened\n");

        return 0;
    }


    if (strcmp(command, "put") == 0) {
        char *key = strtok(NULL, delim);
        char *value = strtok(NULL, "\n");
        if (key == NULL) {
            printf("-> No key specified\n");
            return 0;
        }
        if (value == NULL) {
            printf("-> No value specified\n");
            return 0;
        }
        int success = kv_put(kvs, key, value);
        printf("-> Putting value %s in %s", value, key);
        if (success == 0) {
            log_command(kvs, full_command);
            printf("\n-> Putting successfull\n");
            return 2;
        }
        printf("\n->Putting is NOT successfull\n");

        return 0;
    }


    if (strcmp(command, "get") == 0) {
        char *key = strtok(NULL, delim);
        if (key == NULL) {
            printf("-> No key specified\n");
            return 0;
        }
        const char *value = kv_get(kvs, key);
        if (value == NULL) {
            printf("ERROR: key doesnt exist\n");
            return 0;
        }
        printf("ENTRY: [%s]-{%s}\n", key, value);

        return 3;
    }

    if (strcmp(command, "pstore") == 0) {
        kv_print_store(kvs);
        return 4;
    }

    if (strcmp(command, "close") == 0) {
        kv_print_store(kvs);
        kv_close(kvs);
        return -1;
    }

    if (strcmp(command, "optlog") == 0) {
        int success = optimize_log(kvs);
        if (success == 0) {
            printf("Log successfuly optimized!\n");
            return 0;
        }
        return 1;
    }

    printf("That is not a command!\n");
    return 0;

}
