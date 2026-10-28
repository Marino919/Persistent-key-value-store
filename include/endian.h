#ifndef PERSISTENT_KEY_VALUE_STORE_ENDIAN_H
#define PERSISTENT_KEY_VALUE_STORE_ENDIAN_H

#include "helpers.h"

void put_u32_le(uint8_t *buf, uint32_t v);
uint32_t get_u32_le(const uint8_t *buf);

#endif //PERSISTENT_KEY_VALUE_STORE_ENDIAN_H