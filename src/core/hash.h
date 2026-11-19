/* SPDX-License-Identifier: MIT */
#ifndef TB_CORE_HASH_H
#define TB_CORE_HASH_H

#include "core/types.h"

/* FNV-1a, stable across platforms (used for ids and state checksums). */
uint32_t tb_fnv1a32(const void *data, size_t len);
uint64_t tb_fnv1a64(const void *data, size_t len);
/* Continues an FNV-1a 64 hash with more data. */
uint64_t tb_fnv1a64_update(uint64_t h, const void *data, size_t len);
uint64_t tb_hash_str(const char *s);
/* Integer mixer (splitmix64 finaliser). */
uint64_t tb_hash_u64(uint64_t x);
/* CRC-32 (IEEE), used for save and package checksums. */
uint32_t tb_crc32(const void *data, size_t len);
uint32_t tb_crc32_update(uint32_t crc, const void *data, size_t len);

#endif
