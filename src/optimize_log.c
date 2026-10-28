#include "optimize_log.h"
#include "log_command.h"
#include "helpers.h"

int optimize_log(KVStore *kvs) {
    FILE *temp = fopen("temp.bin", "wb");
    if (temp == NULL) {
        exit(1);
    }

    for (int i = 0; i < kvs->capacity; i++) {
        Entry *e = kvs->buckets[i];
        if (e == NULL) continue;
        while (e != NULL) {
            log_command(temp, OP_PUT, e->key, e->value);
            e = e->entry;
        }
    }

    fflush(temp);
    fclose(temp);
    if (kvs->log) fclose(kvs->log);
    if (rename("temp.bin", kvs->path) != 0) {perror("rename");}
    kvs->log = fopen(kvs->path, "ab");

    return 0;
}
