#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// One BC1 block (8 bytes) as one ASTC 4x4 block (16 bytes); see BC1ToASTC.c.
void PGBC1BlockToASTC(const uint8_t bc1[8], uint8_t astc[16]);
// A region of BC1 blocks (bpr bytes per block row, rpi block rows per image, d images) as ASTC blocks,
// tightly packed; free() the result. NULL if the region doesn't fit the data.
void *PGTranscodeBC1ToASTC(uint32_t w, uint32_t h, uint32_t d, uint32_t bpr, uint32_t rpi, const void *data, size_t size,
                           size_t *outSize, uint32_t *outBpr, uint32_t *outRpi);
#ifdef __cplusplus
}
#endif
