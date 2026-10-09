#include <stdlib.h>

#include "../include/helpers.h"
#include "../include/door.h"

#include <string.h>

#include "command_processor.h"
#include "print_store.h"

KVStore *kv_open( const char *path ) {
    KVStore *kvs = calloc(1, sizeof(KVStore));
    if (kvs == NULL) {
        free(kvs);
        return NULL;
    }
    kvs->capacity = INITIAL_CAPACITY;
    kvs->count = 0;
    kvs->buckets = calloc(INITIAL_CAPACITY, sizeof(Entry *));
    if (kvs->buckets == NULL) {
        free(kvs->buckets);
        free(kvs);
        return NULL;
    }
    kvs->path = dup_string(path);
    kvs->log = fopen(kvs->path, "rb");
    if (kvs->log == NULL) {
        printf("\nERROR: Log file failed to load");
        return kvs;
    }
    char reader[2];
    char *command = calloc(sizeof(char), 1024);
    int nul_guide = 0;
    printf("\nLoading store...");
    while (fread(reader, 1, 1, kvs->log) != 0) {
        reader[1] = '\0';
        //printf("\nREADER: %s", reader);
        command[nul_guide] = reader[0];
        command[nul_guide+1] = '\0';
        //printf("\nCOMMAND: %s", command);
        nul_guide++;
        if (strcmp(reader, "\n") == 0) {
            nul_guide = 0;
            command[strcspn(command, "\n")] = '\0';
            process_command(kvs, command);
        }
    }
    printf("\nStore loaded!\n");
    //kv_print_store(kvs);
    free(command);
    fclose(kvs->log);
    fopen(kvs->path, "ab");

    return kvs;
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
    for (size_t i = 0; i < kvs->capacity; i++) {
        close_chain((kvs->buckets)[i]);
    }
    free(kvs->buckets);
    fclose(kvs->log);
    kvs = NULL;
    free(kvs);
}

