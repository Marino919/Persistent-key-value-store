#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"

int optimize_log(KVStore *kvs) {
    FILE *source = fopen(kvs->path, "rb");

    if (source == NULL) {
        printf("ERROR: file not found");
        return 1;
    }
    FILE *temp = fopen("temp.bin", "wb");
    if (temp == NULL) {
        exit(1);
    }

    char buffer[1024];
    size_t num_lines = 0;
    char **source_lines = calloc(num_lines+1, sizeof(char*));
    if (source_lines == NULL) {
        exit(1);
    }

    while (fgets(buffer, sizeof buffer, source) != 0) {
        buffer[strcspn(buffer, "\n")] = '\0';
        //printf("\ncommand: %s", buffer);
        char *dup_buffer = dup_string(buffer);
        source_lines[num_lines] = dup_buffer;
        num_lines++;
        char **temp_sl = realloc(source_lines, (num_lines+1)*sizeof(char *));
        if (temp_sl == NULL) {
            exit(1);
        }
        source_lines = temp_sl;
    }
    fclose(source);

    for (size_t i = 0; i < num_lines; i++) {

        char *curr_string = source_lines[i];
        if (curr_string == NULL) {
            continue;
        }
        char *dup_curr = dup_string(curr_string);
        char *command = strtok(dup_curr, " \t\n");
        char *key = strtok(NULL, " \t\n");

        if (strcmp(command, "put") == 0) {
            size_t last_put = i;
            for (size_t j = i+1; j < num_lines; j++) {
                if (source_lines[j] == NULL) {
                    continue;
                }
                char *dup_searching_line = dup_string(source_lines[j]);
                char *searching_command = strtok(dup_searching_line, " \t\n");
                char *searching_key = strtok(NULL, " \t\n");

                if (strcmp(searching_command, "put") == 0 && strcmp(searching_key, key) == 0) {
                    free(source_lines[last_put]);
                    source_lines[last_put] = NULL;
                    last_put = j;
                } else if (strcmp(searching_command, "delete") == 0 && strcmp(searching_key, key) == 0) {
                    free(source_lines[last_put]);
                    source_lines[last_put] = NULL;
                    free(source_lines[j]);
                    source_lines[j] = NULL;
                    free(dup_searching_line);
                    break;
                }
                free(dup_searching_line);
            }
        }

        free(dup_curr);
    }

    for (size_t i = 0; i < num_lines; i++) {
        if (source_lines[i] == NULL) {
            continue;
        }
        fprintf(temp, "%s\n", source_lines[i]);

    }
    fflush(temp);
    fclose(temp);
    if (kvs->log) fclose(kvs->log);
    if (rename("temp.bin", kvs->path) != 0) {perror("rename");}
    kvs->log = fopen(kvs->path, "ab");


    for (size_t i = 0; i < num_lines; i++) {
        free(source_lines[i]);
    }
    free(source_lines);
    return 0;
}
