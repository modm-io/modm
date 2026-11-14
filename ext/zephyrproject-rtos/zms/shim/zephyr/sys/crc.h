#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
uint8_t crc8_ccitt(uint8_t initial_value, const void *buf, size_t len);
uint32_t crc32_ieee(const uint8_t *data, size_t len);
uint32_t crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len);
#ifdef __cplusplus
}
#endif
