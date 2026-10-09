#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "door.h"
#include "get.h"
#include "helpers.h"
#include "print_store.h"
#include "put.h"
#include "remove.h"

static void log_command(KVStore *kvs, char *command) {

    const uint32_t len = (uint32_t)strlen(command);
    fwrite(command, sizeof(char), len, kvs->log);
    fflush(kvs->log);
    return ;
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
            printf("\n-> No key specified\n");
            return 0;
        }
        printf("\n-> Deleting key %s", key);
        int success = kv_delete(kvs, key);
        if (success == 0) {
            printf("\n-> key [%s] deleted\n", key);
            log_command(kvs, full_command);
            return 1;
        }
        printf("\n-> key not found or an error happened\n");

        return 0;
    }


    if (strcmp(command, "put") == 0) {
        char *key = strtok(NULL, delim);
        char *value = strtok(NULL, "\n");
        if (key == NULL) {
            printf("\n-> No key specified\n");
            return 0;
        }
        if (value == NULL) {
            printf("\n-> No value specified\n");
            return 0;
        }
        int success = kv_put(kvs, key, value);
        printf("\n-> Putting value %s in %s", value, key);
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
            printf("\n-> No key specified\n");
            return 0;
        }
        const char *value = kv_get(kvs, key);
        if (value == NULL) {
            printf("\n ERROR: key doesnt exist\n");
            return 0;
        }
        printf("\nENTRY: [%s]-{%s}\n", key, value);

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

    return 0;

}
