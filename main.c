#include "kv_basic.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <argument>\n", argv[0]);
        return 1;
    }

    KVStore *kvs= kv_open("log.bin");
    if (kvs == NULL) {
        printf("kv_open FAILED (store opening)");
        return 1;
    }

    char buffer[1024];
    int result = 0;
    while (result != -1 && fgets(buffer, 1024, stdin) != NULL) {

        buffer[strcspn(buffer, "\n")] = '\0';
        result = process_command(kvs, buffer);

    }

    if (result != -1) {
        kv_close(kvs);
    }


    return 0;
}