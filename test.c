#include "test.h"
#include "CRC32Bit.h" //polynomial division version
#include "CRC32Lut.h" //custom polynomial LUT version
#include "CRC32SIMD.h"
#include "stringConversion.h"

//This file is used to test if the three implemented methods work correctly. Also, it prints
//the runtime benchmark results for various input files.
//We created 5 Files: one is the most common example with 43 bytes and the medium one contains 5000 bytes.
//The third one (Big) contains 10000 bytes and the fourth 100000 bytes. Finally, the huge.txt contains 10^12 Bytes.

static const uint32_t iterations = 100;

void test_crc32_file(const char *file_name, uint32_t expected_crc, int x, uint32_t generator) {
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data;
    if ((data = (char *) malloc(len)) == NULL) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    size_t read_bytes = fread(data, 1, len, file);
    if (read_bytes != len) {
        fprintf(stderr, "Read error\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    struct timespec start;
    struct timespec end;
    switch (x) {
        case 0:
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (uint32_t i = 0; i < iterations; i++) {
                CRC32_V0(len, data, reverseUnsigned(generator));
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            break;

        case 1:
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (uint32_t i = 0; i < iterations; i++) {
                CRC32_V1(len, data, generator);
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            break;
        case 2:
            clock_gettime(CLOCK_MONOTONIC, &start);
            for (uint32_t i = 0; i < iterations; i++) {
                CRC32_V2(len, data, reverseUnsigned(generator));
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            break;
        default:
            printf("Error, only 3 versions\n");
            break;
    }

    uint32_t crc;
    if (x == 0) {
        crc = CRC32_V0(len, data, reverseUnsigned(generator));
    } else if (x == 1) {
        crc = CRC32_V1(len, data, generator);
    } else {
        crc = CRC32_V2(len, data, reverseUnsigned(generator));
    }
    free(data);
    if (crc != expected_crc) {
        fprintf(stderr, "Expected 0x%"PRIx32" but calculated result is 0x%"PRIx32"\n", expected_crc, crc);
        fprintf(stderr, "Input file: %s, length %zu\n", file_name, len);
        exit(EXIT_FAILURE);
    }
    double time = (end.tv_sec - start.tv_sec) * 1e3 + 1e-6 * (end.tv_nsec - start.tv_nsec);
    double average = time / iterations;

    printf("Version: %d | Input: %-11s | Duration: %lf msec\n", x, file_name, average);
}

int main(void) {
    //In these tests we use the standard generator
    //Testing the LUT version, force method to create new LUT by using a different polynomial
    printf("Running benchmark, %"PRIu32" iterations for each version\n", iterations);

    printf("LUT version, with table creation:\n");
    test_crc32_file("small.txt", 0x1c8ab2e3,0,0x1234567);
    test_crc32_file("medium.txt", 0x8c8de690,0,0x1234567);
    test_crc32_file("Big.txt", 0xddd7717e,0,0x1234567);
    test_crc32_file("veryBig.txt", 0xfafdb43f,0,0x1234567);
    test_crc32_file("huge.txt", 0x4d354819,0,0x1234567);

    //Testing the default version using LUT and precalculated values
    printf("LUT version, with pre calculated table:\n");
    test_crc32_file("small.txt", 0x414FA339, 0, 0x04C11DB7);
    test_crc32_file("medium.txt", 0x6626A335, 0, 0x04C11DB7);
    test_crc32_file("Big.txt", 0x15f5c4e5, 0, 0x04C11DB7);
    test_crc32_file("veryBig.txt", 0xc4ac1f36, 0, 0x04C11DB7);
    test_crc32_file("huge.txt", 0xae9a202b, 0, 0x04C11DB7);
    
    //Testing the polynomial division version
    printf("Polynomial division with default generator:\n");
    test_crc32_file("small.txt", 0x414FA339, 1, 0x04C11DB7);
    test_crc32_file("medium.txt", 0x6626A335, 1, 0x04C11DB7);
    test_crc32_file("Big.txt", 0x15f5c4e5, 1, 0x04C11DB7);
    test_crc32_file("veryBig.txt", 0xc4ac1f36, 1, 0x04C11DB7);
    test_crc32_file("huge.txt", 0xae9a202b, 1, 0x04C11DB7);

    //Testing the polynomial division version with a different generator
    printf("Polynomial division with alternative generator:\n");
    test_crc32_file("small.txt", 0x1c8ab2e3,1,0x1234567);
    test_crc32_file("medium.txt", 0x8c8de690,1,0x1234567);
    test_crc32_file("Big.txt", 0xddd7717e,1,0x1234567);
    test_crc32_file("veryBig.txt", 0xfafdb43f,1,0x1234567);
    test_crc32_file("huge.txt", 0x4d354819,1,0x1234567);
    
    //This test is an edge case as the input is an empty file and the three methods should return 0x0
    printf("Edge case: empty file -> three methods should return 0x0\n");
    test_crc32_file("empty.txt", 0x0, 0, 0x04C11DB7);
    test_crc32_file("empty.txt", 0x0, 1, 0x04C11DB7);
    test_crc32_file("empty.txt", 0x0, 2, 0x04C11DB7);

    printf("SIMD version, with pre calculated table:\n");
    test_crc32_file("small.txt", 0x414FA339, 2, 0x04C11DB7);
    test_crc32_file("medium.txt", 0x6626A335, 2, 0x04C11DB7);
    test_crc32_file("Big.txt", 0x15f5c4e5, 2, 0x04C11DB7);
    test_crc32_file("veryBig.txt", 0xc4ac1f36, 2, 0x04C11DB7);
    test_crc32_file("huge.txt", 0xae9a202b, 2, 0x04C11DB7);

    printf("SIMD version, with table creation:\n");
    test_crc32_file("small.txt", 0x1c8ab2e3,2,0x1234567);
    test_crc32_file("medium.txt", 0x8c8de690,2,0x1234567);
    test_crc32_file("Big.txt", 0xddd7717e,2,0x1234567);
    test_crc32_file("veryBig.txt", 0xfafdb43f,2,0x1234567);
    test_crc32_file("huge.txt", 0x4d354819,2,0x1234567);
    printf("Tests passed!\n");
    return 0;
}
