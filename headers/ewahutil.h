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


#if defined __UINT32_MAX__ or UINT32_MAX
// unclear whether we need these headers:
//#include <stdint.h>
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


#ifdef _MSC_VER
#include <intrin.h>
/**
 * count the number of bits set to one (32 bit version)
 */
uint32_t countOnes(uint32_t x) {
    return __popcnt((x);
}
#else
/**
 * count the number of bits set to one (32 bit version)
 */
uint32_t countOnes(uint32_t x) {
    return static_cast<uint32_t>(__builtin_popcount(x));
}
#endif


/**
 * count the number of bits set to one (64 bit version)
 */
uint32_t countOnes(uint64_t v) {
    return countOnes(static_cast<uint32_t> (v)) + countOnes(
            static_cast<uint32_t> (v >> 32));
}

uint32_t countOnes(uint16_t v) {
    return countOnes(static_cast<uint32_t>(v));
}

#ifdef _MSC_VER
#include <intrin.h>

// contributed by Christopher Batty
int numberOfTrailingZeros(uint32_t x) {
    unsigned long r = 0;
    if(_BitScanReverse(&r, x))// specific to MS compilers
        return 32;
    return r;
}
#else
int numberOfTrailingZeros(uint32_t x) {
    if (x == 0) return 32;
    return __builtin_ctz(x);// specific to GCC
}
#endif

int numberOfTrailingZeros(uint64_t x) {
    if(static_cast<uint32_t> (x)!= 0) {
        return numberOfTrailingZeros(static_cast<uint32_t> (x));
    }
    else return 32+numberOfTrailingZeros(static_cast<uint32_t> (x >> 32));
}

#ifdef _MSC_VER
// contributed by Christopher Batty
int numberOfTrailingZeros(uint16_t x) {
  unsigned long r = 0;
  if(x == 0) return 16;
  _BitScanReverse(&r, static_cast<uint32_t>(x));// specific to MS compilers
  return r;
}
#else
int numberOfTrailingZeros(uint16_t x) {
    if (x == 0) return 16;
    return __builtin_ctz(x);// specific to GCC
}
#endif

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
