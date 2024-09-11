#ifndef TEAM139_CRC32SIMD_H
#define TEAM139_CRC32SIMD_H
#include <stdio.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <inttypes.h>
#include <string.h>
void calculateTable(uint32_t *table, uint32_t generator);
//void resetTable(void);
uint32_t CRC32_V2(size_t len, const char msg[len], uint32_t generator);
#endif //TEAM139_CRC32SIMD_H
