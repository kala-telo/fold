#ifndef READ_HELPERS_H
#define READ_HELPERS_H
#include <stdint.h>
#include <endian.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

static inline uint8_t read8(FILE *f) {
    uint8_t b;
    fread(&b, 1, 1, f);
    return b;
}
static inline uint16_t read16l(FILE *f) {
    uint16_t b;
    fread(&b, 2, 1, f);
    return le16toh(b);
}
static inline uint16_t read16b(FILE *f) {
    uint16_t b;
    fread(&b, 2, 1, f);
    return be16toh(b);
}
static inline uint16_t read16(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read16b(f);
    case ENDIANESS_LITTLE:
        return read16l(f);
    }
    abort();
    return -1;
}
static inline uint32_t read32l(FILE *f) {
    uint32_t b;
    fread(&b, 4, 1, f);
    return le32toh(b);
}
static inline uint32_t read32b(FILE *f) {
    uint32_t b;
    fread(&b, 4, 1, f);
    return be32toh(b);
}
static inline uint32_t read32(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read32b(f);
    case ENDIANESS_LITTLE:
        return read32l(f);
    }
    abort();
    return -1;
}
static inline uint64_t read64l(FILE *f) {
    uint64_t b;
    fread(&b, 8, 1, f);
    return le64toh(b);
}
static inline uint64_t read64b(FILE *f) {
    uint64_t b;
    fread(&b, 8, 1, f);
    return be64toh(b);
}
static inline uint64_t read64(FILE *f, Endianess e) {
    switch (e) {
    case ENDIANESS_BIG:
        return read64b(f);
    case ENDIANESS_LITTLE:
        return read64l(f);
    }
    abort();
    return -1;
}

static inline uint64_t read32or64(FILE *f, Width w, Endianess e) {
    return (w == WIDTH_32 ? read32(f, e) : read64(f, e));
}
#endif
