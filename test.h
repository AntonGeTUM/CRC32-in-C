#ifndef TEAM139_TEST_H
#define TEAM139_TEST_H
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>
void test_crc32_file(const char* file_name, uint32_t expected_crc, int x, uint32_t generator);
int main(void);
#endif //TEAM139_TEST_H