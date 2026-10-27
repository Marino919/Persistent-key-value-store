#include <stdint.h>
#include <string.h>

#include "endian.h"
#include "helpers.h"
#define DATA_FIELD_N 9


//returns -1 or 0
int log_command(FILE *log, uint8_t operation, const char *key, const char *value){

    uint8_t key_len[4] = {0};
    uint8_t value_len[4] = {0};
    if (key == NULL) return 1;
    uint32_t key_len_32 = strlen(key);
    uint32_t value_len_32 = value ? strlen(value) : 0;
    put_u32_le(key_len, key_len_32);
    put_u32_le(value_len, value_len_32);

    uint8_t data[DATA_FIELD_N] = {operation,
                        key_len[0], key_len[1], key_len[2], key_len[3],
                        value_len[0], value_len[1], value_len[2], value_len[3]};//only header

    if (fwrite(data, sizeof(uint8_t), DATA_FIELD_N, log) != DATA_FIELD_N) return -1;
    if (fwrite(key, sizeof(uint8_t), key_len_32, log) != key_len_32) return -1;
    if (value_len_32 > 0) {
        if (fwrite(value, sizeof(uint8_t), value_len_32, log) != value_len_32) return -1;
    }

    if (fflush(log) != 0) return -1;

    return 0;
}
