#include <string.h>

#include "door.h"
#include "get.h"
#include "helpers.h"
#include "print_store.h"
#include "put.h"
#include "remove.h"

void process_command(KVStore *kvs, char buffer[1024]) {
    const char *delim = " ";
    char *command = strtok(buffer, delim);

    printf("\ncommand given for executing: %s", command);

    if (strcmp(command, "delete") == 0) {
        char *key = strtok(NULL, delim);
        if (key == NULL) {
            printf("\n-> No key specified");
            return;
        }
        printf("\n-> Deleting key %s", key);
        int success = kv_delete(kvs, key);
        if (success == 0) {
            printf("\n-> key [%s] deleted", key);
        } else {
            printf("\n-> key not found"); // possible bug
        }
        return ;
    }


    if (strcmp(command, "put") == 0) {
        char *key = strtok(NULL, delim);
        char *value = strtok(NULL, delim);
        if (key == NULL) {
            printf("\n-> No key specified");
            return ;
        }
        if (value == NULL) {
            printf("\n-> No value specified");
            return ;
        }
        int success = kv_put(kvs, key, value);
        printf("\n-> Putting value %s in %s", value, key);
        if (success == 0) {
            printf("\n-> Putting successfull");
        } else {
            printf("\n->Putting is NOT successfull");
        }
        return;
    }


    if (strcmp(command, "get") == 0) {
        char *key = strtok(NULL, delim);
        if (key == NULL) {
            printf("\n-> No key specified");
            return;
        }
        const char *value = kv_get(kvs, key);
        if (value == NULL) {
            printf("\n ERROR: key doesnt exist");
        } else {
            printf("\nENTRY: [%s]-{%s}", key, value);
        }
        return;
    }


    if (strcmp(command, "close") == 0) {
        kv_print_store(kvs);
        kv_close(kvs);
    }
}
