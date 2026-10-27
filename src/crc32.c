#include <stddef.h>
#include <stdint.h>

uint32_t crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (int i = 0; i < len; i++) {
        crc ^=data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 0x1 == 1) crc = (crc>>1) ^ 0xEDB88320;
            else crc = (crc>>1);
        }
    }
    crc ^= 0xFFFFFFFF;
    return crc;
}
