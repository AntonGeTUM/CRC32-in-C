#ifndef TEAM139_CRC32LUT_H
#define TEAM139_CRC32LUT_H
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
uint32_t CRC32_V0(size_t len, const char msg[len], uint32_t generator);
#endif
