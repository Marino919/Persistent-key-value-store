#include "endian.h"

void put_u32_le(uint8_t *buf, uint32_t v) { //making little endian number (on every machine)
    for (int i =0; i < 4; i++) {
        uint32_t byte_from_end = (v>>(i*8)) & 0xFF;
        buf[i] = byte_from_end;
    }
}

uint32_t get_u32_le(const uint8_t *buf) {
    uint32_t v = 0;
    for (int i = 0; i < 4; i++) {
        v = v << 8;
        v = v | buf[3-i];
    }
    return v;
}