#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "crc32.h"
#include "endian.h"
#include "helpers.h"


//returns -1 or 0
int log_command(FILE *log, uint8_t operation, const char *key, const char *value){
    uint8_t key_len[4] = {0};
    uint8_t value_len[4] = {0};
    uint32_t key_len_32;
    uint32_t value_len_32;
    uint8_t *unprotected_data = NULL;
    uint8_t *protected_data = NULL;
    int ok = -1;

    if (key == NULL) goto done;

    if (strlen(key) > MAX_KEY_LEN) goto done;

    key_len_32 = strlen(key);
    value_len_32 = value ? strlen(value) : 0;
    if (value_len_32>MAX_VALUE_LEN) goto done;

    put_u32_le(key_len, key_len_32);
    put_u32_le(value_len, value_len_32);

    uint8_t unprotected_header[HEADER_N-4] = {operation,
                        key_len[0], key_len[1], key_len[2], key_len[3],
                        value_len[0], value_len[1], value_len[2], value_len[3]};//only header
    size_t unpr_data_n = HEADER_N - 4 + key_len_32 + value_len_32;
    unprotected_data = calloc(unpr_data_n, sizeof(uint8_t));
    size_t prot_data_n = HEADER_N + key_len_32 + value_len_32;
    protected_data = calloc(prot_data_n, sizeof(uint8_t));

    if (unprotected_data == NULL) goto done;
    if (protected_data == NULL) goto done;

    memcpy(unprotected_data, unprotected_header, HEADER_N-4);
    memcpy(unprotected_data+HEADER_N-4, key, key_len_32);
    memcpy(unprotected_data+HEADER_N-4+key_len_32, value, value_len_32);

    uint8_t crc8[4] = {0};
    uint32_t crc32_code = crc32(unprotected_data, unpr_data_n);
    put_u32_le(crc8, crc32_code);

    memcpy(protected_data, crc8, 4);
    memcpy(protected_data+4, unprotected_data, unpr_data_n);

    if (fwrite(protected_data, sizeof(uint8_t), prot_data_n, log) != prot_data_n) goto done;
    if (fflush(log) != 0) goto done;


    ok = 0;

    done:
    free(unprotected_data);
    free(protected_data);
    return ok;
}
