/* SPDX-License-Identifier: MIT */
#include "core/hash.h"

#include <string.h>

#define FNV32_OFFSET 2166136261u
#define FNV32_PRIME  16777619u
#define FNV64_OFFSET 14695981039346656037ull
#define FNV64_PRIME  1099511628211ull

uint32_t tb_fnv1a32(const void *data, size_t len)
{
    const uint8_t *p = data;
    uint32_t h = FNV32_OFFSET;
    for (size_t i = 0; i < len; i++) {
        h = (h ^ p[i]) * FNV32_PRIME;
    }
    return h;
}

uint64_t tb_fnv1a64_update(uint64_t h, const void *data, size_t len)
{
    const uint8_t *p = data;
    for (size_t i = 0; i < len; i++) {
        h = (h ^ p[i]) * FNV64_PRIME;
    }
    return h;
}

uint64_t tb_fnv1a64(const void *data, size_t len)
{
    return tb_fnv1a64_update(FNV64_OFFSET, data, len);
}

uint64_t tb_hash_str(const char *s)
{
    return tb_fnv1a64(s, strlen(s));
}

uint64_t tb_hash_u64(uint64_t x)
{
    x ^= x >> 30;
    x *= 0xbf58476d1ce4e5b9ull;
    x ^= x >> 27;
    x *= 0x94d049bb133111ebull;
    x ^= x >> 31;
    return x;
}

uint32_t tb_crc32_update(uint32_t crc, const void *data, size_t len)
{
    const uint8_t *p = data;
    crc = ~crc;
    for (size_t i = 0; i < len; i++) {
        crc ^= p[i];
        for (int k = 0; k < 8; k++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

uint32_t tb_crc32(const void *data, size_t len)
{
    return tb_crc32_update(0, data, len);
}
