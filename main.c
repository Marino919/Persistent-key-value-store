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
        process_command(kvs, buffer);
    }

    return 0;
}