#ifndef PERSISTENT_KEY_VALUE_STORE_CDC32_H
#define PERSISTENT_KEY_VALUE_STORE_CDC32_H
#include <stddef.h>
#include <stdint.h>

uint32_t crc32(const uint8_t *data, size_t len);

#endif //PERSISTENT_KEY_VALUE_STORE_CDC32_H