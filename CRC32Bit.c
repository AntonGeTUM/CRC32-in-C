#include "CRC32Bit.h"
#include "stringConversion.h"

/** Calculates the CRC32 checksum using the polynomial division method. Following the standard for most calculations
 *  the initial CRC value is set to 0xFFFFFFFF, the input is reflected and the return value is first XORed with
 *  0xFFFFFFFF and then reflected one last time.
 *  Return value: CRC32 checksum as uint32_t **/
uint32_t CRC32_V1(size_t len, char* msg, uint32_t generator) {
    if(len==0){
        return 0;
    }
    //initial crc value
    uint32_t crc = 0xFFFFFFFF;
    //mask to check for the most significant bit
    uint32_t mask = 0x80000000;

    for (size_t i = 0; i < len; i++) {
        //reflect current input byte
        char temp = reflectChar(msg[i]);
        //load current input byte: shift by 24 to fill uint32_t, then XOR
        crc = (crc ^ (temp << 24));

        for (size_t j = 0; j < 8; j++) {
            if ((crc & mask)) {
                //if the most significant bit has been set, shift then XOR with the whole polynomial
                crc = (crc << 1) ^ generator;
            } else {
                //otherwise just shift
                crc <<= 1;
            }
        }
    }
    crc ^= 0xffffffff;
    crc = reverseUnsigned(crc);
    return crc;
}