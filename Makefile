FLAGS = -O2 -Wall -Wextra

all: crc32

crc32:
		gcc $(FLAGS) -o crc32 main.c CRC32Lut.c stringConversion.c CRC32Bit.c CRC32SIMD.c
		gcc $(FLAGS) -o test test.c CRC32Bit.c CRC32Lut.c stringConversion.c CRC32SIMD.c

clean:
		rm -rf crc32
		rm -rf test