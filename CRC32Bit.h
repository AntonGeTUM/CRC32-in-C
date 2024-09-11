#ifndef TEAM139_CRC32BIT_H
#define TEAM139_CRC32BIT_H
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
char reflectChar(char input);
uint32_t CRC32_V1(size_t len, char* msg, uint32_t generator);
#endif //TEAM139_CRC32BIT_H
