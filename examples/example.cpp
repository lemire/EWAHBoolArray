/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include <stdlib.h>
#include "ewah/ewah.h"
using namespace ewah;

void easydemo() {
  typedef EWAHBoolArray<uint32_t> bitmap;
  bitmap bitset1 =
      bitmap::bitmapOf(9, 1, 2, 1000, 1001, 1002, 1003, 1007, 1009, 100000);
  std::cout << "first bitset : " << bitset1 << std::endl;
  bitmap bitset2 = bitmap::bitmapOf(5, 1, 3, 1000, 1007, 100000);
  std::cout << "second bitset : " << bitset2 << std::endl;
  bitmap bitset3 = bitmap::bitmapOf(3, 10, 11, 12);
  std::cout << "third  bitset : " << bitset3 << std::endl;
  bitmap orbitset = bitset1 | bitset2;
  bitmap andbitset = bitset1 & bitset2;
  bitmap xorbitset = bitset1 ^ bitset2;
  bitmap andnotbitset = bitset1 - bitset2;

  std::cout << "logical and: " << andbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << andbitset.sizeInBytes()
            << " bytes" << std::endl;
  // we will display the and
  std::cout << "logical or: " << orbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << orbitset.sizeInBytes()
            << " bytes" << std::endl;
  // we will display the xor
  std::cout << "logical xor: " << xorbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << xorbitset.sizeInBytes()
            << " bytes" << std::endl;
  std::cout << "union of all three bitsets = " << (orbitset | bitset3)
            << std::endl;
  const bitmap *mybitmaps[] = {&bitset1, &bitset2, &bitset3};
  std::cout << "fast union of all three bitsets = "
            << fast_logicalor(3, mybitmaps) << std::endl;

  std::cout << std::endl;
}

template <class bitmap> void demo() {
  bitmap bitset1 =
      bitmap::bitmapOf(9, 1, 2, 1000, 1001, 1002, 1003, 1007, 1009, 100000);
  std::cout << "first bitset : " << bitset1 << std::endl;
  bitmap bitset2 = bitmap::bitmapOf(5, 1, 3, 1000, 1007, 100000);
  std::cout << "second bitset : " << bitset2 << std::endl;
  bitmap bitset3 = bitmap::bitmapOf(3, 10, 11, 12);
  std::cout << "third  bitset : " << bitset3 << std::endl;
  bitmap orbitset;
  bitmap andbitset;
  bitmap xorbitset;
  bitset1.logicalor(
      bitset2,
      orbitset); // "bitset1.logicalor(bitset2)" would also return orbitset
  bitset1.logicaland(bitset2, andbitset);
  bitset1.logicalxor(bitset2, xorbitset);
  // we will display the or
  std::cout << "logical and: " << andbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << andbitset.sizeInBytes()
            << " bytes" << std::endl;
  // we will display the and
  std::cout << "logical or: " << orbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << orbitset.sizeInBytes()
            << " bytes" << std::endl;
  // we will display the xor
  std::cout << "logical xor: " << xorbitset << std::endl;
  std::cout << "memory usage of compressed bitset = " << xorbitset.sizeInBytes()
            << " bytes" << std::endl;
  std::cout << "union of all three bitsets = " << bitset1.logicalor(bitset2)
            << std::endl;
  const bitmap *mybitmaps[] = {&bitset1, &bitset2, &bitset3};
  std::cout << "fast union of all three bitsets = "
            << fast_logicalor(3, mybitmaps) << std::endl;

  std::cout << std::endl;
}

template <class bitmap> void demoSerialization() {
  std::stringstream ss;
  bitmap myarray;
  myarray.addWord(234321); // this is not the same as "set(234321)"!!!
  myarray.addWord(0);
  myarray.addWord(0);
  myarray.addWord(999999);
  //
  std::cout << "Writing: " << myarray << std::endl;
  myarray.write(ss);
  //
  bitmap lmyarray;
  lmyarray.read(ss);
  std::cout << "Read back: " << lmyarray << std::endl;
  //
  if (lmyarray == myarray)
    std::cout << "serialization works" << std::endl;
  else
    std::cout << "serialization does not works" << std::endl;
}

template <class bitmap> void smallIntersectBenchmark() {
  // this is the worst case!!!
  bitmap b1;
  bitmap b2;
  // we fill both of them with lots of values, but we
  // do not let them intersect, to make it difficult!
  size_t N = 100000000;
  for (size_t i = 0; i < N; ++i) {
    b1.set(2 * i);
    b2.set(2 * i + 1);
  }
  size_t offset = 2 * N + 2;
  for (size_t i = 0; i < 100000; ++i) {
    for (size_t j = 0; j < 1024; ++j)
      b1.set(offset + j);
    offset += 1024;
    for (size_t j = 0; j < 1024; ++j)
      b2.set(offset + j);
    offset += 1024;
  }
  std::cout << " First bitmap has " << b1.numberOfOnes() << " set bits"
            << std::endl;
  std::cout << " Second bitmap has " << b2.numberOfOnes() << " set bits"
            << std::endl;
  double inputsize = b1.numberOfOnes() + b2.numberOfOnes();
  const clock_t START = clock();
  if (b1.intersects(b2))
    std::cout << "BUG!" << std::endl;
  const clock_t END = clock();
  double timee = (double)(END - START) / CLOCKS_PER_SEC;
  std::cout << "Billions of values intersected per second : "
            << (inputsize / 1000000000.0) / timee << std::endl;
}

int main(void) {
  std::cout << std::endl;
  std::cout << "====uncompressed example====" << std::endl;
  std::cout << std::endl;
  demo<BoolArray<uint32_t>>();
  demoSerialization<BoolArray<uint32_t>>();

  std::cout << std::endl;
  std::cout << "====compressed example====" << std::endl;
  std::cout << std::endl;
  easydemo();

  demo<EWAHBoolArray<uint32_t>>();
  demoSerialization<EWAHBoolArray<uint32_t>>();
  std::cout << "==== benchmark intersecs === " << std::endl;
  smallIntersectBenchmark<EWAHBoolArray<uint64_t>>();
  return EXIT_SUCCESS;
}
