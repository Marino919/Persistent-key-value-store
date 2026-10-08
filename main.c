#include "kv_basic.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <argument>\n", argv[0]);
        return 1;
    }
    KVStore *kvs= kv_open(argv[1]);
    if (kvs == NULL) {
        printf("kv_open FAILED (store opening)");
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, 1024, stdin) != NULL) {

        buffer[strcspn(buffer, "\n")] = '\0';
        const char *delim = " ";
        char *command = strtok(buffer, delim);

        printf("\ncommand given for executing: %s", command);

        if (strcmp(command, "delete") == 0) {
            char *key = strtok(NULL, delim);
            if (key == NULL) {
                printf("\n-> No key specified");
                continue;
            }
            printf("\n-> Deleting key %s", key);
            int success = kv_delete(kvs, key);
            if (success == 0) {
                printf("\n-> key [%s] deleted", key);
            } else {
                printf("\n-> key not found"); // possible bug
            }
            continue;
        }


        if (strcmp(command, "put") == 0) {
            char *key = strtok(NULL, delim);
            char *value = strtok(NULL, delim);
            if (key == NULL) {
                printf("\n-> No key specified");
                continue;
            }
            if (value == NULL) {
                printf("\n-> No value specified");
                continue;
            }
            int success = kv_put(kvs, key, value);
            printf("\n-> Putting value %s in %s", value, key);
            if (success == 0) {
                printf("\n-> Putting successfull");
            } else {
                printf("\n->Putting is NOT successfull");
            }
            continue;
        }


        if (strcmp(command, "get") == 0) {
            char *key = strtok(NULL, delim);
            if (key == NULL) {
                printf("\n-> No key specified");
                continue;
            }
            const char *value = kv_get(kvs, key);
            if (value == NULL) {
                printf("\n ERROR: key doesnt exist");
            } else {
                printf("\nENTRY: [%s]-{%s}", key, value);
            }
            continue;
        }


        if (strcmp(command, "close") == 0) {
            kv_print_store(kvs);
            kv_close(kvs);
        }
    }

    return 0;
}