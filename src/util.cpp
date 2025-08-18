#include "util.h"

#include <cstdint>

/*
* The Hamming distance is the number of bit positions where the two values differ.
*/
int hammingDistance(const std::uint8_t a, const uint8_t b) {
    uint8_t x = a ^ b; // XOR
    uint8_t count = 0;

    while (x) {
        x &= x - 1;
        ++count;
    }

    return count;
}
