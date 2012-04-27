/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef EWAHUTIL_H
#define EWAHUTIL_H

#include <string.h>
#include <stdlib.h>
#include <iso646.h> // mostly for Microsoft compilers
#include <inttypes.h>

#ifdef UINT32_MAX
#include <stdint.h> // for old compilers?
//#include <cstdint>
#else
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
#endif
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <sstream>

// taken from stackoverflow
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

/**
 * count the number of bits set to one (32 bit version)
 */
uint32_t countOnes(uint32_t v) {
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}
/**
 * count the number of bits set to one (64 bit version)
 */
uint32_t countOnes(uint64_t v) {
    return countOnes(static_cast<uint32_t> (v)) + countOnes(
            static_cast<uint32_t> (v >> 32));
}

uint32_t countOnes(uint16_t v) {
    uint32_t c;
    for (c = 0; v; c++) {
        v &= v - 1;
    }
    return c;
}

/**
 * Returns the binary representation of a binary word.
 */
template<class uword>
std::string toBinaryString(const uword w) {
    std::ostringstream convert;
    for (uint32_t k = 0; k < sizeof(uword) * 8; ++k) {
        if (w & (static_cast<uword> (1) << k))
            convert << "1";
        else
            convert << "0";
    }
    return convert.str();
}

#endif
