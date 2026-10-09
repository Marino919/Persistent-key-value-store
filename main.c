#include <stdlib.h>

#include "kv_basic.h"

int main(void) {

    char *storename = calloc(1019, sizeof(char));
    if (storename == NULL) {
        return 1;
    }
    printf("Open store: ");
    if (scanf("%1018s", storename) != 1) {
        printf("\n No name given!");
        free(storename);
        return 1;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    char *storename_bin = realloc(storename, 1024);
    if (storename_bin == NULL) {
        free(storename);
        return 1;
    }
    strcat(storename_bin, ".bin");
    KVStore *kvs= kv_open(storename_bin);
    free(storename_bin);
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