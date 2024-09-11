#include "stringConversion.h"

//Helper method to reverse the order of bits in a 32-bit integer
uint32_t reverseUnsigned(uint32_t number) {
    uint32_t res = 0;
    for (int i = 0; i < 32; i++) {
        res = res << 1;
        if ((number & 1) == 1) {
            res += 1;
        }
        number = number >> 1;
    }
    return res;
}

//Helper method to reverse the order of bits in a char
char reflectChar(char input) {
    char res = 0;
    for (int i = 0; i < 8; i++) {
        res <<= 1;
        if (input & 1) {
            res += 1;
        }
        input >>= 1;
    }
    return res;
}

//Helper method to convert strings to integer
int64_t convertStringToInt(char* input) {
    int64_t temp;
    char* end;
    temp = strtol(input, &end, 0);
    if (temp > 0xffffffff || *end != 0 || temp < 0) {
        return -1;
    }
    return temp;
}
