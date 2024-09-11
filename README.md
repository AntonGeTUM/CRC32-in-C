Explanation of what CRC or cyclic redundancy check is: see [wiki](https://en.wikipedia.org/wiki/Cyclic_redundancy_check)

The task of the project was to
  1. create a basic CRC-32 implementation in C
  2. create alternative/optimized versions
  3. run a benchmark to compare all implementations.

All versions are required to accept input as the first argument and calculate a checksum for its content. An optional second argument can be used to specify a custom generator polynomial.

The first optimized version uses a look-up table to speed up the checksum calculation.
**(missing source, will add later)**

A third implementation uses SIMD instructions in an attempt to speed up the creation of the LUT for custom generator polynomials.
