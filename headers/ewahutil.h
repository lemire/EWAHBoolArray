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
#include <limits.h>
#include <stdint.h> // part of Visual Studio 2010 and better

//#if defined __UINT32_MAX__ or UINT32_MAX
//#include <inttypes.h>
//#else
//#if UCHAR_MAX == 0xffff
//typedef unsigned char uint16_t;
//#elif USHRT_MAX == 0xffff
//typedef unsigned short uint16_t;
//#elif UINT_MAX == 0xffff
//typedef unsigned int uint16_t;
//#endif
//#if UINT_MAX == 0xffffffff
//typedef unsigned int uint32_t;
//#elif USHRT_MAX == 0xffffffff
//typedef unsigned short uint32_t;
//#elif ULONG_MAX == 0xffffffff
//typedef unsigned long uint32_t;
//#endif
//#if ULONG_LONG_MAX == 0xffffffffffffffff
//typedef unsigned long long uint64_t;
//#elif UINT_MAX == 0xffffffffffffffff
//typedef unsigned int uint64_t;
//#elif ULONG_MAX == 0xffffffffffffffff
//typedef unsigned long uint64_t;
//#endif
//#endif
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


#ifdef __GNUG__
/**
 * count the number of bits set to one (32 bit version)
 */
uint32_t countOnes(uint32_t x) {
    return static_cast<uint32_t>(__builtin_popcount(x));
}
#else
uint32_t countOnes(uint32_t x) {
  uint32_t c; // c accumulates the total bits set in v
  for (c = 0; x; c++) {
     x &= x - 1; // clear the least significant bit set
  }
  return c;
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
#ifdef __GNUG__
int numberOfTrailingZeros(uint32_t x) {
    if (x == 0) return 32;
    return __builtin_ctz(x);// specific to GCC
}
#else
int numberOfTrailingZeros(uint32_t v) {
	if(v == 0) return 32;
    int c = 1;
    if ((v & 0xffff) == 0) {  
      v >>= 16;  
      c += 16;
    }
    if ((v & 0xff) == 0) {  
      v >>= 8;  
      c += 8;
    }
    if ((v & 0xf) == 0) {  
      v >>= 4;
      c += 4;
    }
    if ((v & 0x3) == 0) {  
      v >>= 2;
      c += 2;
    }
    c -= v & 0x1;
    return c;
}	
#endif

int numberOfTrailingZeros(uint64_t x) {
    if(static_cast<uint32_t> (x)!= 0) {
        return numberOfTrailingZeros(static_cast<uint32_t> (x));
    }
    else return 32+numberOfTrailingZeros(static_cast<uint32_t> (x >> 32));
}

#ifdef __GNUG__
int numberOfTrailingZeros(uint16_t x) {
    if (x == 0) return 16;
    return __builtin_ctz(x);// specific to GCC
}
#else
int numberOfTrailingZeros(uint16_t v) {
  if(v == 0) return 16;
  int c = 1;
  if ((v & 0xff) == 0) {  
      v >>= 8;  
      c += 8;
  }
  if ((v & 0xf) == 0) {  
      v >>= 4;
      c += 4;
  }
  if ((v & 0x3) == 0) {  
      v >>= 2;
      c += 2;
  }
  c -= v & 0x1;
  return c;
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
