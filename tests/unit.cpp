/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include "ewah/boolarray.h"
#include "ewah/ewah.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#define SSTR(x) (to_string(x))

using namespace std;

static string testfailed = "---\ntest failed.\n\n\n\n\n\n";

// for Microsoft compilers
#if _MSC_VER >= 1400
#define unlink _unlink
#endif
template <class uword> bool testAndNotCompactionEWAHBoolArray() {
  cout << "[testing testAndNotCompactionEWAHBoolArray] sizeof(uword)="
       << sizeof(uword) << endl;
  EWAHBoolArray<uword> one;
  EWAHBoolArray<uword> other;
  one.set(71);
  other.set(130);
  other = other.logicaland(one);
  //other is zero
 // one = one.logicalandnot(other);
  // one remained on change {71}
  one.set(155);
  //one has {71,155}
    other.debugprintout();

  other.set(251);
  //other.debugprintout();
  abort();
  return  251 == *other.begin();
  //other has {131}
//    std::cout <<  " {251}==" << other << std::endl;
/*

  other = other.logicalor(one);
  // {71,131,155}
  one = one.logicaland(other);
  // {155}

  std::cout << "one";
  one.debugprintout();
  std::cout << "other";
  other.debugprintout();
  std::cout << other << std::endl;
    std::cout << one << std::endl;
  other = other.logicalandnot(one);

  std::cout << "other";
  other.debugprintout();
  typename EWAHBoolArray<uword>::const_iterator i = other.begin();
  auto val = *i;
  std::cout << "got " << val << std::endl;
  return val == 217;*/
  return true;
}

template <class uword> bool testInEqualityEWAHBoolArray() {
  cout << "[testing testInEqualityEWAHBoolArray] sizeof(uword)="
       << sizeof(uword) << endl;
  EWAHBoolArray<uword> b1;
  EWAHBoolArray<uword> b = EWAHBoolArray<uword>::bitmapOf(3, 1, 10, 11);
  EWAHBoolArray<uword> b3 = EWAHBoolArray<uword>::bitmapOf(3, 1, 10, 11);
  EWAHBoolArray<uword> b4 = EWAHBoolArray<uword>::bitmapOf(4, 1, 10, 11, 10000);

  if (b3 != b) {
    cout << "should be equal!" << endl;
    return false;
  }
  if (b == b1) {
    cout << "should be different!" << endl;
    return false;
  }
  if (b4 == b1) {
    cout << "should be different!" << endl;
    return false;
  }
  if (b4 == b3) {
    cout << "should be different!" << endl;
    return false;
  }
  return true;
}
template <class uword> bool testEmpty() {
  cout << "[testing SizeInBits] sizeof(uword)=" << sizeof(uword) << endl;
  EWAHBoolArray<uword> ewah0;
  if (!ewah0.empty()) {
    std::cout << "empty is buggy " << std::endl;
    return false;
  }
  EWAHBoolArray<uword> ewah1 = EWAHBoolArray<uword>::bitmapOf(1, 1);
  if (ewah1.empty()) {
    std::cout << "empty is buggy " << std::endl;
    return false;
  }
  return true;
}
template <class uword> bool testSizeInBits() {
  cout << "[testing SizeInBits] sizeof(uword)=" << sizeof(uword) << endl;

  EWAHBoolArray<uword> ewah1 = EWAHBoolArray<uword>::bitmapOf(1, 1);
  EWAHBoolArray<uword> ewah2 = EWAHBoolArray<uword>::bitmapOf(1, 2);
  EWAHBoolArray<uword> ewah3 = EWAHBoolArray<uword>::bitmapOf(1, 3);
  EWAHBoolArray<uword> ewah4 = EWAHBoolArray<uword>::bitmapOf(1, 4);
  EWAHBoolArray<uword> result1 = ewah1 | ewah2 | ewah3 | ewah4;
  EWAHBoolArray<uword> result12 = ewah1 & ewah2 & ewah3 & ewah4;
  EWAHBoolArray<uword> result13 = ewah1 ^ ewah2 ^ ewah3 ^ ewah4;

  EWAHBoolArray<uword> result2 = EWAHBoolArray<uword>::bitmapOf(4, 1, 2, 3, 4);
  if (result1 != result2) {
    cout << testfailed << endl;
    std::cout << "should be equal " << std::endl;
    return false;
  }
  if ((result1.sizeInBits() != result2.sizeInBits()) ||
      (result12.sizeInBits() != result2.sizeInBits()) ||
      (result13.sizeInBits() != result2.sizeInBits())) {
    cout << testfailed << endl;
    std::cout << "bad sizeInBits " << std::endl;
    return false;
  }
  return true;
}

template <class uword> bool testSerialSize() {
  cout << "[testing SerialSize] sizeof(uword)=" << sizeof(uword) << endl;
  size_t bitval = 55555;
  EWAHBoolArray<uword> b =
      EWAHBoolArray<uword>::bitmapOf(1, bitval); // set just one bit
  // generic write method in EWAHBoolArray can be wasteful for single bits...
  stringstream ss;
  b.write(ss);
  size_t compressedsize = ss.tellp();
  const size_t wordinbits = sizeof(uword) * CHAR_BIT;
  size_t offset = (bitval + wordinbits) / wordinbits - 1;
  offset = (offset + RunningLengthWord<uword>::largestrunninglengthcount - 1) /
           RunningLengthWord<uword>::largestrunninglengthcount;
  size_t expectedsize =
      2 * sizeof(size_t) + sizeof(uword) + offset * sizeof(uword);
  cout << "using " << compressedsize << " bytes " << endl;
  if (compressedsize != expectedsize) {
    cout << "bad size, was expected  " << expectedsize << " got "
         << compressedsize << endl;
    return false; // unexpected size
  }
  ss.str("");
  ss.clear();
  //
  uint32_t buffersize =
      (uint32_t)b.bufferSize(); // I am going to use only 32-bit
  ss.write(reinterpret_cast<const char *>(&buffersize), sizeof(uint32_t));
  b.writeBuffer(ss);
  compressedsize = ss.tellp();
  expectedsize = sizeof(uint32_t) + sizeof(uword) + offset * sizeof(uword);
  cout << "using " << compressedsize << " bytes with custom format" << endl;
  if (compressedsize != expectedsize) {
    cout << "bad size, was expected  " << expectedsize << " got "
         << compressedsize << endl;
    return false; // unexpected size
  }
  return true;
}

template <class uword> bool testCardinalityBoolArray() {
  cout << "[testing CardinalityBoolArray] sizeof(uword)=" << sizeof(uword)
       << endl;
  BoolArray<uword> b1 = BoolArray<uword>::bitmapOf(1, 1);
  if (b1.numberOfOnes() != 1) {
    return false;
  }
  b1.inplace_logicalnot();
  if (b1.numberOfOnes() != 1) {
    return false;
  }

  BoolArray<uword> b = BoolArray<uword>::bitmapOf(2, 1, 100);
  if (b.numberOfOnes() != 2) {
    return false;
  }
  BoolArray<uword> bout;
  b.logicalnot(bout);
  if (bout.numberOfOnes() != 99) {
    return false;
  }
  b.inplace_logicalnot();
  if (b.numberOfOnes() != 99) {
    return false;
  }
  return true;
}

template <class uword> bool testAndNotBoolArray() {
  cout << "[testing AndNotBoolArray] sizeof(uword)=" << sizeof(uword) << endl;
  BoolArray<uword> b1 = BoolArray<uword>::bitmapOf(1, 1);
  BoolArray<uword> b = BoolArray<uword>::bitmapOf(2, 1, 100);
  BoolArray<uword> bout;
  b.logicalandnot(b1, bout);
  if (bout.numberOfOnes() != 1) {
    return false;
  }
  return true;
}

template <class uword> bool testIntersects() {
  cout << "[testing intersects] sizeof(uword)=" << sizeof(uword) << endl;
  cout << "constructing b1..." << endl;

  EWAHBoolArray<uword> b1 = EWAHBoolArray<uword>::bitmapOf(2, 2, 1000);
  cout << "constructing b2..." << endl;

  EWAHBoolArray<uword> b2 = EWAHBoolArray<uword>::bitmapOf(2, 1000, 1100);
  cout << "constructing b3..." << endl;

  EWAHBoolArray<uword> b3 = EWAHBoolArray<uword>::bitmapOf(2, 1100, 2000);
  cout << "About to intersect..." << endl;
  cout << "all of them: " << b1.intersects(b2) << " " << b2.intersects(b3)
       << " " << b1.intersects(b3) << "done" << endl;
  if (!b1.intersects(b2))
    return false;
  if (!b2.intersects(b3))
    return false;
  if (b1.intersects(b3))
    return false;
  return true;
}

template <class uword> bool testCardinalityEWAHBoolArray() {
  cout << "[testing CardinalityEWAHBoolArray] sizeof(uword)=" << sizeof(uword)
       << endl;
  EWAHBoolArray<uword> b1 = EWAHBoolArray<uword>::bitmapOf(1, 1);
  if (b1.numberOfOnes() != 1) {
    return false;
  }
  b1.inplace_logicalnot();
  if (b1.numberOfOnes() != 1) {
    cout << "b1 " << b1 << endl;
    return false;
  }

  EWAHBoolArray<uword> b = EWAHBoolArray<uword>::bitmapOf(2, 1, 100);
  if (b.numberOfOnes() != 2) {
    cout << "b " << b << endl;
    return false;
  }
  EWAHBoolArray<uword> bout;
  b.logicalnot(bout);
  if (bout.numberOfOnes() != 99) {
    cout << "bout " << bout << endl;
    return false;
  }
  b.inplace_logicalnot();
  if (b.numberOfOnes() != 99) {
    cout << "b neg " << b << endl;
    return false;
  }
  return true;
}

template <class uword> bool testLargeDirty() {
  cout << "[testing LargeDirty] sizeof(uword)=" << sizeof(uword) << endl;
  size_t N = 200000000;
  vector<uint32_t> bigarray1(N);
  vector<uint32_t> bigarray2(N);
  for (size_t k = 0; k < N; k++) {
    bigarray1[k] = (uint32_t)(8 * k);
    bigarray2[k] = (uint32_t)(32 * k);
  }
  EWAHBoolArray<uword> b1 = EWAHBoolArray<uword>();
  EWAHBoolArray<uword> b2 = EWAHBoolArray<uword>();
  for (size_t k = 0; (8 * k) < N; k++) {
    b1.set(8 * k);
  }
  for (size_t k = 0; (64 * k) < N; k++) {
    b1.set(64 * k);
  }

  EWAHBoolArray<uword> b3 = EWAHBoolArray<uword>::bitmapOf(3, 1, 10, 1001);
  EWAHBoolArray<uword> b4 = EWAHBoolArray<uword>::bitmapOf(3, 1, 10, 1001);

  if (b3.numberOfOnes() != 3) {
    std::cout << "bad b3 count" << b4.numberOfOnes() << std::endl;
    return false;
  }
  b3 = b3 | b1;
  b4 = b4 | b2;
  b3 = b3 | b2;
  b4 = b4 | b1;
  if (b3 != b4) {
    std::cout << "b3 != b4" << std::endl;
    return false;
  }
  b4 = b4 - b1;
  if (b4.numberOfOnes() != 3) {
    std::cout << "bad b4 count" << b4.numberOfOnes() << std::endl;
    return false;
  }
  b3 = b3 ^ b4;
  if (b3 != b1) {
    std::cout << "b3 != b1" << std::endl;
    return false;
  }

  return true;
}

template <class uword> bool testAndNotEWAHBoolArray() {
  cout << "[testing AndNotEWAHBoolArray] sizeof(uword)=" << sizeof(uword)
       << endl;
  EWAHBoolArray<uword> b1 = EWAHBoolArray<uword>::bitmapOf(1, 1);
  EWAHBoolArray<uword> b = EWAHBoolArray<uword>::bitmapOf(2, 1, 100);
  EWAHBoolArray<uword> bout;
  b.logicalandnot(b1, bout);
  cout << bout << endl;
  if (bout.numberOfOnes() != 1) {
    cout << "expected answer : 1 " << endl;
    return false;
  }
  return true;
}

template <class uword> bool testNanJiang() {
  cout << "[testing NanJiang] sizeof(uword)=" << sizeof(uword) << endl;
  EWAHBoolArray<uword> b;
  b.set(5);
  if (b.numberOfOnes() != 1)
    return false;
  b.inplace_logicalnot();
  if (b.numberOfOnes() != 5)
    return false;
  BoolArray<uword> b2;
  b2.set(5);
  if (b2.numberOfOnes() != 1)
    return false;
  b2.inplace_logicalnot();
  if (b2.numberOfOnes() != 5)
    return false;
  return true;
}

template <class uword> bool testGet() {
  cout << "[testing Get] sizeof(uword)=" << sizeof(uword) << endl;
  bool isOk = true;

  for (size_t gap = 29; gap < 10000; gap *= 10) {
    EWAHBoolArray<uword> x;
    for (uint32_t k = 0; k < 100; ++k)
      x.set(k * gap);
    for (size_t k = 0; k < 100 * gap; ++k)
      if (x.get(k)) {
        if (k % gap != 0) {
          cout << "spotted an extra set bit at " << k << " gap = " << gap
               << endl;
          return false;
        }
      } else if (k % gap == 0) {
        cout << "missed a set bit " << k << " gap = " << gap << endl;
        return false;
      }
  }
  return isOk;
}

template <class uword> bool testLucaDeri() {
  cout << "[testing LucaDeri] sizeof(uword)=" << sizeof(uword) << endl;
  bool isOk = true;
  EWAHBoolArray<uword> bitset1;
  bitset1.set(1);
  bitset1.set(2);
  bitset1.set(2);
  bitset1.set(1000);
  bitset1.set(1001);
  if (bitset1.numberOfOnes() != 4) {
    cout << "Failed LucaDeri test" << endl;
    isOk = false;
  }
  return isOk;
}

template <class uword> bool testSetGet() {
  cout << "[testing EWAH set/get] sizeof(uword)=" << sizeof(uword) << endl;
  EWAHBoolArray<uword> ewcb;
  uint32_t val[] = {5, 4400, 44600, 55400, 1000000};
  for (int k = 0; k < 5; ++k) {
    ewcb.set(val[k]);
  }
  size_t counter = 0;
  bool isOk = true;
  for (typename EWAHBoolArray<uword>::const_iterator i = ewcb.begin();
       i != ewcb.end(); ++i) {
    if (val[counter++] != *i) {
      cout << "Failed test set/get" << endl;
      isOk = false;
    }
  }
  return isOk;
}

template <class uword> bool testRunningLengthWord() {
  cout << "[testing RunningLengthWord]" << endl;
  bool isOk(true);
  uword somenumber(0xABCD);
  RunningLengthWord<uword> rlw(somenumber);
  rlw.setRunningBit(true);
  if (rlw.getRunningBit() != true) {
    cout << "failed to set the running bit " << sizeof(uword) << endl;
    isOk = false;
  }
  for (uword myrl = 0;
       myrl <= RunningLengthWord<uword>::largestrunninglengthcount;
       myrl = static_cast<uword>(
           myrl + RunningLengthWord<uword>::largestrunninglengthcount / 10)) {
    rlw.setRunningLength(myrl);
    if (rlw.getRunningBit() != true) {
      cout << "failed to set the running bit (2) " << sizeof(uword) << endl;
      isOk = false;
    }
    if (rlw.getRunningLength() != myrl) {
      cout << "failed to set the running length " << sizeof(uword) << endl;
      isOk = false;
    }
  }
  rlw.setRunningLength(12);
  for (uword mylw = 0; mylw <= RunningLengthWord<uword>::largestliteralcount;
       mylw = static_cast<uword>(
           mylw + RunningLengthWord<uword>::largestliteralcount / 10)) {
    rlw.setNumberOfLiteralWords(mylw);
    if (rlw.getRunningBit() != true) {
      cout << "failed to set the running bit (3) " << sizeof(uword) << endl;
      isOk = false;
    }
    if (rlw.getRunningLength() != 12) {
      cout << "failed to set the running length (2) " << sizeof(uword) << endl;
      isOk = false;
    }
    if (rlw.getNumberOfLiteralWords() != mylw) {
      cout << "failed to set the LiteralWords " << mylw << " " << sizeof(uword)
           << " " << rlw.getNumberOfLiteralWords() << endl;
      isOk = false;
    }
  }
  rlw.setNumberOfLiteralWords(43);
  rlw.setRunningBit(false);
  if (rlw.getRunningBit() != false) {
    cout << "failed to set the running bit (4) " << sizeof(uword) << endl;
    isOk = false;
  }
  if (rlw.getRunningLength() != 12) {
    cout << "failed to set the running length (3) " << sizeof(uword) << endl;
    isOk = false;
  }
  if (rlw.getNumberOfLiteralWords() != 43) {
    cout << "failed to set the LiteralWords (2) " << sizeof(uword) << endl;
    isOk = false;
  }
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

template <class uword> bool testEWAHBoolArrayAppend() {
  cout << "[testing EWAHBoolArrayAppend]" << endl;
  bool isOk(true);
  uword zero = 0;
  uword specialval =
      1UL + (1UL << 4) + (static_cast<uword>(1) << (sizeof(uword) * 8 - 1));
  uword notzero = static_cast<uword>(~zero);
  EWAHBoolArray<uword> myarray1;
  BoolArray<uword> ba1;
  myarray1.addWord(zero);
  ba1.addWord(zero);
  myarray1.addWord(zero);
  ba1.addWord(zero);
  myarray1.addWord(zero);
  ba1.addWord(zero);
  myarray1.addWord(specialval);
  ba1.addWord(specialval);
  myarray1.addWord(specialval);
  ba1.addWord(specialval);
  myarray1.addWord(notzero);
  ba1.addWord(notzero);
  myarray1.addWord(zero);
  ba1.addWord(zero);
  EWAHBoolArray<uword> myarray2;
  BoolArray<uword> ba2;
  myarray2.addWord(notzero);
  ba2.addWord(notzero);
  myarray2.addWord(zero);
  ba2.addWord(zero);
  myarray2.addWord(notzero);
  ba2.addWord(notzero);
  myarray2.addWord(specialval);
  ba2.addWord(specialval);
  myarray2.addWord(specialval);
  ba2.addWord(specialval);
  myarray2.addWord(notzero);
  ba2.addWord(notzero);
  BoolArray<uword> aggregate1(ba1);
  BoolArray<uword> aggregate2(ba2);
  aggregate1.append(ba2);
  aggregate2.append(ba1);
  EWAHBoolArray<uword> caggregate1;
  caggregate1.append(myarray1);
  EWAHBoolArray<uword> caggregate2;
  caggregate2.append(myarray2);
  caggregate1.append(myarray2);
  caggregate2.append(myarray1);
  if (caggregate1 != aggregate1) {
    cout << "aggregate 1 failed" << endl;
    isOk = false;
  }
  if (caggregate2 != aggregate2) {
    cout << "aggregate 2 failed" << endl;
    isOk = false;
  }
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

// unit test contributed by Joerg Bukowski ✆
template <class uword> bool testJoergBukowski() {
  cout << "[testing JoergBukowski]" << endl;
  bool isOk(true);
  vector<uint32_t> positions;
  positions.push_back(0);
  positions.push_back(36778);
  positions.push_back(51863);
  positions.push_back(134946);
  positions.push_back(137330);
  positions.push_back(147726);
  positions.push_back(147990);
  positions.push_back(151884);
  positions.push_back(156404);
  positions.push_back(158486);
  positions.push_back(159622);
  positions.push_back(163159);
  positions.push_back(164599);
  string indexfile("testingewahboolarray.bin");
  ::remove(indexfile.c_str());
  EWAHBoolArray<uword> mytestarray;
  for (vector<uint32_t>::const_iterator i = positions.begin();
       i != positions.end(); ++i) {
    mytestarray.set(*i);
  }
  size_t count = 0;
  for (typename EWAHBoolArray<uword>::const_iterator j = mytestarray.begin();
       j != mytestarray.end(); ++j) {
    if (count >= positions.size()) {
      std::cout << " Should be done by now!" << std::endl;
      isOk = false;
      assert(false);
      break;
    }
    if (*j != positions[count]) {
      isOk = false;
      std::cout << " Expected " << positions[count] << std::endl;
      break;
    }
    count++;
  }
  if (count < positions.size()) {
    std::cout << "counted " << count << " values but expected "
              << positions.size() << std::endl;
    isOk = false;
  }
  assert(count == positions.size());
  EWAHBoolArray<uword> myarray;
  for (vector<uint32_t>::const_iterator i = positions.begin();
       i != positions.end(); ++i) {
    myarray.set(*i);
    ofstream out(indexfile.c_str(), ios::out | ios::binary);
    myarray.write(out);
    out.close();
    EWAHBoolArray<uword> recovered;
    ifstream in(indexfile.c_str(), ios::binary);
    recovered.read(in);
    in.close();
    vector<size_t> vals;
    recovered.appendSetBits(vals);
    if (vals.size() != static_cast<size_t>(i - positions.begin() + 1)) {
      cout << "failed to recover right number" << endl;
      isOk = false;
    }
    if (!equal(vals.begin(), vals.end(), positions.begin())) {
      cout << "failed to recover" << endl;
      isOk = false;
    }
    vals.clear();
    for (typename EWAHBoolArray<uword>::const_iterator j = recovered.begin();
         j != recovered.end(); ++j) {
      vals.push_back(static_cast<uint32_t>(*j));
    }
    if (vals.size() != static_cast<size_t>(i - positions.begin() + 1)) {
      cout << "failed to recover right number -- iterator" << endl;
      isOk = false;
    }
    if (!equal(vals.begin(), vals.end(), positions.begin())) {
      cout << "failed to recover -- iterator" << endl;
      isOk = false;
    }
  }
  if (isOk)
    ::remove(indexfile.c_str());
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

// unit test contributed by Phong Tran
bool testPhongTran() {
  cout << "[testing PhongTran]" << endl;
  bool isOk(true);
  EWAHBoolArray<uint32_t> myarray;
  for (uint32_t x = 0; x < 10000; x++) {
    myarray.addWord(x);
  }
  string indexfile("testingewahboolarray.bin");
  ::remove(indexfile.c_str());
  ofstream out(indexfile.c_str(), ios::out | ios::binary);
  myarray.write(out);
  out.close();
  EWAHBoolArray<uint32_t> lmyarray;
  ifstream in(indexfile.c_str(), ios::binary);
  lmyarray.read(in);
  in.close();
  EWAHBoolArrayIterator<uint32_t> i = myarray.uncompress();
  EWAHBoolArrayIterator<uint32_t> j = lmyarray.uncompress();
  while (i.hasNext() or j.hasNext()) {
    if ((!j.hasNext()) or (!i.hasNext())) {
      cout << "the two arrays don't have the same size?" << endl;
      isOk = false;
      break;
    }
    uint32_t val = i.next();
    uint32_t val2 = j.next();
    if (val != val2) {
      cout << "the two arrays differ" << endl;
      isOk = false;
      break;
    }
  }

  if (isOk)
    ::remove(indexfile.c_str());
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

// another unit test contributed by Phong Tran
template <class uword> bool testHemeury() {
  cout << "[testing Hemeury]" << endl;
  bool isOk(true);
  EWAHBoolArray<uword> test, test1, test2;
  for (uint32_t i = 0; i <= 10000; ++i) {
    test.set(i);
    test.logicaland(test1, test2);
    // because test1 is empty, test2 should be empty as well
    vector<size_t> vals;
    test2.appendSetBits(vals);
    if (vals.size() != 0) {
      isOk = false;
      cout << "failed!" << endl;
      break;
    }
  }
  return isOk;
}

// another unit test contributed by Phong Tran
template <class uword> bool testPhongTran2() {
  cout << "[testing PhongTran2]" << endl;
  bool isOk(true);
  uword iTotal = static_cast<uword>(
      1000); // when 1000 does not fit in uword, then it will be casted
  EWAHBoolArray<uword> myarray;
  for (uword x = static_cast<uword>(100); x < iTotal; x++) {
    myarray.addWord(x);
  }
  string indexfile("testingewahboolarray.bin");
  ::remove(indexfile.c_str());
  ofstream out(indexfile.c_str(), ios::out | ios::binary);
  myarray.write(out);
  out.close();
  EWAHBoolArray<uword> lmyarray;
  ifstream in(indexfile.c_str(), ios::binary);
  lmyarray.read(in);
  in.close();
  if (!(myarray == lmyarray)) {
    cout << "bad news, they are not equal" << endl;
    cout << "size in bits: " << myarray.sizeInBits() << " vs. "
         << lmyarray.sizeInBits() << endl;
    isOk = false;
  }
  EWAHBoolArrayIterator<uword> i = myarray.uncompress();
  EWAHBoolArrayIterator<uword> j = lmyarray.uncompress();
  while (i.hasNext()) {
    if (!j.hasNext()) {
      cout << "the two arrays don't have the same size?" << endl;
      isOk = false;
      break;
    }
    uword val = i.next();
    uword val2 = j.next();
    if (val != val2) {
      cout << "the two arrays differ " << endl;
      isOk = false;
    }
  }
  if (isOk)
    ::remove(indexfile.c_str());
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

template <class uword> bool testEWAHBoolArray() {
  cout << "[testing EWAHBoolArray]" << endl;
  bool isOk(true);
  EWAHBoolArray<uword> myarray;
  BoolArray<uword> ba(10 * sizeof(uword) * 8);
  uword zero = 0;
  uword notzero = static_cast<uword>(~zero);
  myarray.addWord(zero);
  ba.setWord(0, zero);
  myarray.addWord(zero);
  ba.setWord(1, zero);
  myarray.addWord(zero);
  ba.setWord(2, zero);
  uword specialval =
      1UL + (1UL << 4) + (static_cast<uword>(1) << (sizeof(uword) * 8 - 1));
  myarray.addWord(specialval);
  ba.setWord(3, specialval);
  myarray.addWord(notzero);
  ba.setWord(4, notzero);
  myarray.addWord(notzero);
  ba.setWord(5, notzero);
  myarray.addWord(notzero);
  ba.setWord(6, notzero);
  myarray.addWord(notzero);
  ba.setWord(7, notzero);
  myarray.addWord(specialval);
  ba.setWord(8, specialval);
  myarray.addWord(zero);
  ba.setWord(9, zero);
  if (myarray.sizeInBits() != 10 * sizeof(uword) * 8) {
    cout << "expected " << 10 * sizeof(uword) * 8 << " bits but found "
         << myarray.sizeInBits() << endl;
    isOk = false;
  }
  string indexfile("testingewahboolarray.bin");
  ::remove(indexfile.c_str());
  ofstream out(indexfile.c_str(), ios::out | ios::binary);
  myarray.write(out);
  out.close();
  EWAHBoolArray<uword> lmyarray;
  ifstream in(indexfile.c_str(), ios::binary);
  lmyarray.read(in);
  in.close();
  if (!(myarray == lmyarray)) {
    cout << "bad news, they are not equal" << endl;
    cout << "size in bits: " << myarray.sizeInBits() << " vs. "
         << lmyarray.sizeInBits() << endl;
    isOk = false;
  }
  EWAHBoolArrayIterator<uword> i = myarray.uncompress();
  EWAHBoolArrayIterator<uword> j = lmyarray.uncompress();
  uint32_t k = 0;
  while (i.hasNext()) {
    if (!j.hasNext()) {
      cout << "the two arrays don't have the same size?" << endl;
      isOk = false;
      break;
    }
    uword val = i.next();
    uword val2 = j.next();
    uword valref = ba.getWord(k++);
    if (val != valref) {
      cout << "the two arrays differ from uncompressed array at " << k << " "
           << val << " " << val2 << " " << valref << endl;
      isOk = false;
    }
    if (val != val2) {
      cout << "the two arrays differ at " << k << " " << val << " " << val2
           << " " << valref << endl;
      isOk = false;
    }
  }
  if (isOk)
    ::remove(indexfile.c_str());
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

template <class uword> bool testNot() {
  cout << "[testing Not]" << endl;
  bool isOk = true;
  EWAHBoolArray<uword> bitset;
  for (int i = 0; i <= 184; i++) {
    bitset.set(i);
  }
  if (bitset.numberOfOnes() != 185) {
    isOk = false;
  }

  bitset.inplace_logicalnot();
  if (bitset.numberOfOnes() != 0) {
    isOk = false;
  }
  return isOk;
}

template <class uword> bool testSTLCompatibility() {
  cout << "[testing STL compatibility]" << endl;
  bool isOk = true;
  EWAHBoolArray<uword> bitset1;
  bitset1.set(1);
  bitset1.set(2);
  bitset1.set(1000);
  bitset1.set(1001);
  vector<EWAHBoolArray<uword>> testVec(1);
  testVec[0].set(1);
  testVec[0].set(2);
  testVec[0].set(1000);
  testVec[0].set(1001);
  if (testVec[0] != bitset1) {
    isOk = false;
  }
  return isOk;
}

template <class uword> bool testEWAHBoolArrayLogical() {
  cout << "[testing EWAHBoolArrayLogical] word size = " << sizeof(uword)
       << endl;
  bool isOk(true);
  EWAHBoolArray<uword> myarray1;
  EWAHBoolArray<uword> myarray2;

  uword allones = static_cast<uword>(~0LL);
  const uint32_t N = 16;
  uword x1[N] = {1,       0,       54,      24, 145, 0, 0, 0,
                 allones, allones, allones, 43, 0,   0, 0, 1};
  uword x2[N] = {allones, 1,       0,       0,       0, 0, 0, 0,
                 0,       allones, allones, allones, 0, 4, 0, 0};
  uword xand[N];
  uword xxor[N];
  size_t usedN = 10;
  if (sizeof(uword) > 2)
    return true;

  for (uint32_t k = 0; k < usedN; ++k) {
    myarray1.addWord(x1[k]);
    myarray2.addWord(x2[k]);
    xand[k] = static_cast<uword>(x1[k] & x2[k]);
    xxor[k] = static_cast<uword>(x1[k] | x2[k]);
  }
  EWAHBoolArray<uword> myand;
  EWAHBoolArray<uword> myor;
  EWAHBoolArray<uword> myxor;
  EWAHBoolArray<uword> myxoralt;

  myarray1.logicaland(myarray2, myand);
  myarray1.logicalor(myarray2, myor);
  myarray1.logicalxor(myarray2, myxor);
  EWAHBoolArray<uword> tmp(myand);
  tmp.inplace_logicalnot();
  myor.logicaland(tmp, myxoralt);
  if (myxoralt != myxor) {
    isOk = false;
    if (!isOk)
      cout << testfailed << endl;
    return isOk;
  }

  EWAHBoolArrayIterator<uword> i = myand.uncompress();
  EWAHBoolArrayIterator<uword> j = myor.uncompress();
  EWAHBoolArrayIterator<uword> it1 = myarray1.uncompress();
  EWAHBoolArrayIterator<uword> it2 = myarray2.uncompress();
  for (uint32_t k = 0; k < usedN; ++k) {
    const uword m1 = it1.next();
    const uword m2 = it2.next();
    if (!i.hasNext()) {
      if ((m1 & m2) != 0) {
        cout << "type 1 error" << endl;
        isOk = false;
        break;
      }
    } else {
      const uword inter = i.next();
      if (inter != xand[k]) {
        cout << "type 4 error" << endl;
        isOk = false;
        break;
      }
    }
    if (!j.hasNext()) {
      if ((m1 | m2) != 0) {
        cout << "type 3 error" << endl;
        isOk = false;
        break;
      }
    } else {
      const uword jor = j.next();
      if (jor != xxor[k]) {
        cout << "type 6 error OR" << endl;
        isOk = false;
        break;
      }
    }
  }
  if (!isOk)
    cout << testfailed << endl;
  return isOk;
}

#define N_ENTRIES(arr) (sizeof(arr) / sizeof((arr)[0]))

template <class uword>
void init(EWAHBoolArray<uword> &ba, size_t N, size_t x[]) {
  for (size_t ix = 0; ix < N; ++ix)
    ba.set(x[ix]);
}

template <class uword>
std::ostream &operator<<(std::ostream &os, const EWAHBoolArray<uword> &ba) {
  os << " (" << ba.sizeInBits() << ") ";
  typename EWAHBoolArray<uword>::const_iterator it = ba.begin(),
                                                last = ba.end();
  for (int ix = 0; it != last; ++it, ++ix) {
    if (ix > 0)
      os << ", ";
    os << *it;
  }
  os << endl;

  size_t ixBit = 0;
  const uword wordInBits = EWAHBoolArray<uword>::wordinbits;

  EWAHBoolArrayRawIterator<uword> ir = ba.raw_iterator();
  for (int jx = 0; ir.hasNext(); ++jx) {
    BufferedRunningLengthWord<uword> &brlw(ir.next());
    string tf = (brlw.getRunningBit() ? "true" : "false");
    size_t runBits = static_cast<size_t>(brlw.getRunningLength() * wordInBits);
    size_t litBits =
        static_cast<size_t>(brlw.getNumberOfLiteralWords() * wordInBits);
    os << jx << ", " << ixBit << ": " << tf << " for "
       << brlw.getRunningLength() << " words(" << runBits << " bits), "
       << brlw.getNumberOfLiteralWords() << " literals (" << litBits << " bits)"
       << endl;
    ixBit += (runBits + litBits);
  }
  string eq = (ixBit == ba.sizeInBits() ? "==" : "!=");
  os << "[" << ixBit << eq << ba.sizeInBits() << "]" << endl;
  return os;
}

template <class uword> bool testSerialization() {
  cout << "[testing Serialization] word size = " << sizeof(uword) << endl;
  EWAHBoolArray<uword> bitmap;
  for (int i = 0; i < (1 << 30); i = 2 * i + 3) {
    bitmap.set(static_cast<size_t>(i));
  }
  stringstream ss;
  EWAHBoolArray<uword> lmyarray;
  for (int k = 0; k < 10; ++k) {
    size_t w1 = bitmap.write(ss);
    if (w1 != bitmap.sizeOnDisk()) {
      return false;
    }
    size_t w2 = lmyarray.read(ss);
    if (w2 != bitmap.sizeOnDisk()) {
      return false;
    }
    if (lmyarray != bitmap) {
      return false;
    }
    typename EWAHBoolArray<uword>::const_iterator i = bitmap.begin();
    typename EWAHBoolArray<uword>::const_iterator j = lmyarray.begin();
    for (; i != bitmap.end(); ++i, ++j) {
      if (*i != *j)
        return false;
    }
  }
  return true;
}

template <class uword> bool testRawSerialization() {
  cout << "[testing Raw Serialization] word size = " << sizeof(uword) << endl;
  EWAHBoolArray<uword> bitmap;
  for (int i = 0; i < (1 << 30); i = 2 * i + 3) {
    bitmap.set(static_cast<size_t>(i));
  }
  vector<char> buffer(bitmap.sizeOnDisk());
  stringstream ss;
  EWAHBoolArray<uword> lmyarray;
  for (int k = 0; k < 10; ++k) {
    size_t w1 = bitmap.write(buffer.data(), buffer.size());
    if (w1 != bitmap.sizeOnDisk()) {
      return false;
    }
    size_t w2 = lmyarray.read(buffer.data(), buffer.size());
    if (w2 != bitmap.sizeOnDisk()) {
      return false;
    }
    if (lmyarray != bitmap) {
      return false;
    }
    typename EWAHBoolArray<uword>::const_iterator i = bitmap.begin();
    typename EWAHBoolArray<uword>::const_iterator j = lmyarray.begin();
    for (; i != bitmap.end(); ++i, ++j) {
      if (*i != *j)
        return false;
    }
  }
  return true;
}

template <class uword> bool testEWAHBoolArrayLogical2() {
  bool ok = true;
  cout << "[testing EWAHBoolArrayLogical2] word size = " << sizeof(uword)
       << endl;

  EWAHBoolArray<uword> ba1, ba2, baAND, baOR, baXOR, testAND, testOR, testXOR;

  size_t x1[] = {1, 3, 24, 54, 145, 3001, 3002, 3004, 10003};
  size_t x2[] = {2, 3, 22, 57, 199, 3000, 3002, 10003, 999999};
  size_t x1_AND_x2[] = {3, 3002, 10003};
  size_t x1_OR_x2[] = {1,   2,    3,    22,   24,   54,    57,    145,
                       199, 3000, 3001, 3002, 3004, 10003, 999999};
  size_t x1_XOR_x2[] = {1,   2,   22,   24,   54,   57,
                        145, 199, 3000, 3001, 3004, 999999};

  init(ba1, N_ENTRIES(x1), x1);
  init(ba2, N_ENTRIES(x2), x2);
  init(baAND, N_ENTRIES(x1_AND_x2), x1_AND_x2);
  init(baOR, N_ENTRIES(x1_OR_x2), x1_OR_x2);
  init(baXOR, N_ENTRIES(x1_XOR_x2), x1_XOR_x2);

  // Make 'em all the same size in bits, so equality operators should work.
  ba1.makeSameSize(ba2);
  baAND.makeSameSize(ba2);
  baOR.makeSameSize(ba2);
  baXOR.makeSameSize(ba2);

  ba1.logicaland(ba2, testAND);
  ba1.logicalor(ba2, testOR);
  ba1.logicalxor(ba2, testXOR);

  if (baAND != testAND) {
    cout << " AND failed:" << endl;
    cout << "Expected: " << baAND << endl;
    cout << "Encountered: " << testAND << endl;
    ok = false;
  }
  if (baOR != testOR) {
    cout << " OR failed: " << endl;
    cout << "Expected: " << baOR << endl;
    cout << "Encountered: " << testOR << endl;
    ok = false;
  }
  if (baXOR != testXOR) {
    cout << " XOR failed: " << endl;
    cout << "Expected: " << baXOR << endl;
    cout << "Encountered: " << testXOR << endl;
    ok = false;
  }

  // Verify order of operands has no effect on results
  ba2.logicaland(ba1, testAND);
  ba2.logicalor(ba1, testOR);
  ba2.logicalxor(ba1, testXOR);

  if (baAND != testAND) {
    cout << " AND failed (2):" << endl;
    cout << "Expected: " << baAND << endl;
    cout << "Encountered: " << testAND << endl;
    ok = false;
  }
  if (baOR != testOR) {
    cout << " OR failed (2): " << endl;
    cout << "Expected: " << baOR << endl;
    cout << "Encountered: " << testOR << endl;
    ok = false;
  }
  if (baXOR != testXOR) {
    cout << " XOR failed (2): " << endl;
    cout << "Expected: " << baXOR << endl;
    cout << "Encountered: " << testXOR << endl;
    ok = false;
  }
  if (!ok)
    cout << testfailed << endl;
  return ok;
}

template <class uword> bool testFastOrAggregate() {
  bool ok = true;
  cout << "[testing FastOrAggregate] word size = " << sizeof(uword) << endl;

  EWAHBoolArray<uword> ba1, ba2, ba3, baOR;

  size_t x1[] = {1, 3, 24, 54, 145, 3001, 3002, 3004, 10003};
  size_t x2[] = {2, 3, 22, 57, 199, 3000, 3002, 10003, 999999};
  size_t x3[] = {3, 22, 57, 199, 3000, 3002, 10003, 10004, 999999};

  size_t OR[] = {1,   2,    3,    22,   24,   54,    57,    145,
                 199, 3000, 3001, 3002, 3004, 10003, 10004, 999999};

  init(ba1, N_ENTRIES(x1), x1);
  init(ba2, N_ENTRIES(x2), x2);
  init(ba3, N_ENTRIES(x3), x3);
  init(baOR, N_ENTRIES(OR), OR);

  EWAHBoolArray<uword> longway = ba1.logicalor(ba2).logicalor(ba3);
  const EWAHBoolArray<uword> *mybitmaps[] = {&ba3, &ba2, &ba1};
  EWAHBoolArray<uword> fastway =
      fast_logicalor(N_ENTRIES(mybitmaps), mybitmaps);

  if (longway != baOR) {
    cout << " OR failed:" << endl;
    cout << "Expected: " << baOR << endl;
    cout << "Encountered: " << longway << endl;
    ok = false;
  }
  if (fastway != baOR) {
    cout << "fast OR failed:" << endl;
    cout << "Expected: " << baOR << endl;
    cout << "Encountered: " << fastway << endl;
    ok = false;
  }
  if (!ok)
    cout << testfailed << endl;
  return ok;
}

void tellmeaboutmachine() {
  cout << "number of bytes in ostream::pos_type = " << sizeof(ostream::pos_type)
       << endl;
  cout << "number of bytes in size_t = " << sizeof(size_t) << endl;
  cout << "number of bytes in int = " << sizeof(int) << endl;
  cout << "number of bytes in long = " << sizeof(long) << endl;
#if __LITTLE_ENDIAN__
  cout << "you have little endian machine" << endl;
#endif
#if __BIG_ENDIAN__
  cout << "you have a big endian machine" << endl;
#endif
#if __CHAR_BIT__
  if (__CHAR_BIT__ != 8)
    cout << "on your machine, chars don't have 8bits???" << endl;
#endif
#if __GNUG__
  cout << "GNU GCC compiler detected." << endl;
#else
  cout << "Non-GCC compiler." << endl;
#endif
}

// class by  Jacques NABHAN
class JavaEWAHReader {
public:
  JavaEWAHReader() {}

  static EWAHBoolArray<uint64_t> **readFile(string filename) {
    EWAHBoolArray<uint64_t> **ewahs = new EWAHBoolArray<uint64_t> *[2];

    ifstream inputStream;
    inputStream.open(filename.c_str(), ios::binary);

    ewahs[0] = readOneBitmap(&inputStream);
    ewahs[1] = readOneBitmap(&inputStream);

    inputStream.close();

    return ewahs;
  }

  virtual ~JavaEWAHReader() {}

  static EWAHBoolArray<uint64_t> *readOneBitmap(ifstream *inputStream) {
    EWAHBoolArray<uint64_t> *ewah = new EWAHBoolArray<uint64_t>;

    uint32_t sizeInBits = 0;
    inputStream->read((char *)&sizeInBits, 4);
    sizeInBits = swapBytesIfNecessary(sizeInBits);

    uint32_t numberOfOnes = 0;
    inputStream->read((char *)&numberOfOnes, 4);
    numberOfOnes = swapBytesIfNecessary(numberOfOnes);

    uint32_t tmp = 0;
    for (unsigned long i = 0; i < numberOfOnes; ++i) {
      inputStream->read((char *)&tmp, 4);
      tmp = swapBytesIfNecessary(tmp);
      ewah->set(tmp);
    }

    ewah->setSizeInBits(sizeInBits);
    return ewah;
  }

private:
  static uint32_t swapBytesIfNecessary(uint32_t num) {
    int i = 1;
    if (*(char *)&i == 1) {
      // little endian machine
      return ((num >> 24) & 0xff) |      // move byte 3 to byte 0
             ((num << 8) & 0xff0000) |   // move byte 1 to byte 2
             ((num >> 8) & 0xff00) |     // move byte 2 to byte 1
             ((num << 24) & 0xff000000); // byte 0 to byte 3
    } else {
      // big endian machine
      return num;
    }
  }
};
int dirExists(const char *path) {
  struct stat info;

  if (stat(path, &info) != 0)
    return 0;
  else if (info.st_mode & S_IFDIR)
    return 1;
  else
    return 0;
}

bool testRealData(std::string data) {
  cout << "[testRealData] from JavaEWAH bitmaps (Jacques NABHAN)" << endl;

  string path = data + "bitmap_dumps/";
  if (!dirExists(path.c_str())) {
    cout << "I cannot find bitmap dump directory : " << path << endl;
    cout << "Please run unit tests from a proper location. " << endl;
    cout << "For now, real-data tests are disabled. " << endl;

    return true;
  }
  const size_t N = 207;
  vector<size_t> v1, v2, va, vor, vxor, vand;
  EWAHBoolArray<uint64_t> container;

  for (size_t k = 0; k < 207; ++k) {
    cout << ".";
    cout.flush();
    v1.clear();
    v2.clear();
    va.clear();
    vor.clear();
    vxor.clear();
    container.reset();
    string filename = path + SSTR(k * 1000);
    EWAHBoolArray<uint64_t> **ewahs = JavaEWAHReader::readFile(filename);
    ewahs[0]->appendSetBits(v1);
    ewahs[1]->appendSetBits(v2);
    if (ewahs[0] == ewahs[1]) {
      cerr << "successive bitmaps shouldn't be equal" << endl;
      return false;
    }
    size_t c1 = 0;
    for (auto i = ewahs[0]->begin(); i != ewahs[0]->end(); ++i)
      c1++;
    if (c1 != v1.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad iterator size at vec 1" << endl;
      return false;
    }
    if (ewahs[0]->numberOfOnes() != v1.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad size at vec 1" << endl;
      return false;
    }
    size_t c2 = 0;
    for (auto i = ewahs[1]->begin(); i != ewahs[1]->end(); ++i)
      c2++;
    if (c2 != v2.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad iterator size at vec 2" << endl;
      return false;
    }
    if (ewahs[1]->numberOfOnes() != v2.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad size at vec 2" << endl;
      return false;
    }
    size_t predictedintersection = ewahs[0]->logicalandcount(*ewahs[1]);
    ewahs[0]->logicaland(*ewahs[1], container);
    if (container.numberOfOnes() != predictedintersection) {
      cerr << "bad logicalandcount" << endl;
      return false;
    }
    container.appendSetBits(va);
    if (container.numberOfOnes() != va.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad size from intersection" << endl;
      return false;
    }

    vector<size_t> longintersection(v1.size() + v2.size());
    longintersection.resize(std::set_intersection(v1.begin(), v1.end(),
                                                  v2.begin(), v2.end(),
                                                  longintersection.begin()) -
                            longintersection.begin());
    if (longintersection != va) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "intersections do not match!" << endl;
      return false;
    }

    container.reset();
    size_t predictedunion = ewahs[0]->logicalorcount(*ewahs[1]);
    ewahs[0]->logicalor(*ewahs[1], container);
    if (container.numberOfOnes() != predictedunion) {
      cerr << "bad logicalorcount" << endl;
      return false;
    }
    container.appendSetBits(vor);
    if (container.numberOfOnes() != vor.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad size from union" << endl;
      return false;
    }

    vector<size_t> longunion(v1.size() + v2.size());
    longunion.resize(std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
                                    longunion.begin()) -
                     longunion.begin());
    if (longunion != vor) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "unions do not match!" << endl;
      cerr << "They have lengths " << longunion.size() << " and " << vor.size()
           << endl;

      return false;
    }

    container.reset();

    ewahs[0]->logicalxor(*ewahs[1], container);
    size_t predictedxor = ewahs[0]->logicalxorcount(*ewahs[1]);
    if (container.numberOfOnes() != predictedxor) {
      cerr << "bad logicalxorcount" << container.numberOfOnes() << " "
           << predictedxor << endl;
      return false;
    }

    container.appendSetBits(vxor);
    if (container.numberOfOnes() != vxor.size()) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "bad size from xor" << endl;
      return false;
    }

    vector<size_t> longxor(v1.size() + v2.size());
    longxor.resize(std::set_symmetric_difference(v1.begin(), v1.end(),
                                                 v2.begin(), v2.end(),
                                                 longxor.begin()) -
                   longxor.begin());
    if (longxor != vxor) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "xor do not match!" << endl;
      return false;
    }

    EWAHBoolArray<uint64_t> tmpcontainer;

    ewahs[0]->logicalxor(*ewahs[1], tmpcontainer);
    EWAHBoolArray<uint64_t> createdandnotcontainer;
    ewahs[0]->logicaland(tmpcontainer, createdandnotcontainer);
    EWAHBoolArray<uint64_t> directandnotcontainer;
    ewahs[0]->logicalandnot(*ewahs[1], directandnotcontainer);

    if (directandnotcontainer != createdandnotcontainer) {
      cout << "Loading bitmaps from file " << filename << endl;
      cerr << "andnot do not match!" << endl;
      return false;
    }
    size_t predictedandnot = ewahs[0]->logicalandnotcount(*ewahs[1]);
    if (directandnotcontainer.numberOfOnes() != predictedandnot) {
      cerr << "bad logicalandnotcount" << endl;
      return false;
    }

    delete ewahs[0];
    delete ewahs[1];
    delete[] ewahs;
  }
  cout << endl;
  cout << "Tested " << N << " bitmap pairs with success!" << endl;
  return true;
}

template <class uword> bool dataindexingtest(std::string path) {
  cout << "[dataindexingtest] checking intersects...sizeof(uword)="
       << sizeof(uword) << endl;

  // read the data from the CSV file
  vector<string> col1, col2;
  string datasource = path + "data.csv";

  ifstream infile(datasource.c_str(), ios::binary);
  if (!infile) {
    cout << "WARNING: For this test to run, I need to find " << datasource
         << endl;
    return true;
  }

  while (infile) {
    string s;
    if (!getline(infile, s))
      break;

    istringstream ss(s);
    vector<string> record;

    int numCommas = 0;
    while (ss) {
      if (!getline(ss, s, ','))
        break;
      if (numCommas == 0) {
        col1.push_back(s);
        numCommas++;
      } else {
        col2.push_back(s);
        numCommas = 0;
      }
    }
  }

  map<string, EWAHBoolArray<uword>> index1;
  map<string, EWAHBoolArray<uword>> index2;

  for (size_t i = 0; i < col1.size(); i++) {
    index1[col1[i]].set(i);
  }
  for (size_t i = 0; i < col2.size(); i++) {
    index2[col2[i]].set(i);
  }

  size_t testcount = 0;

  for (typename map<string, EWAHBoolArray<uword>>::iterator i = index1.begin();
       i != index1.end(); ++i) {
    EWAHBoolArray<uword> &bitmap1 = i->second;
    for (typename map<string, EWAHBoolArray<uword>>::iterator j =
             index2.begin();
         j != index2.end(); ++j) {
      EWAHBoolArray<uword> &bitmap2 = j->second;
      bool intersect = bitmap1.intersects(bitmap2);
      EWAHBoolArray<uword> result;
      testcount++;
      bitmap1.logicaland(bitmap2, result);
      if (intersect) {
        if (result.numberOfOnes() == 0)
          return false;
      } else {
        if (result.numberOfOnes() > 0)
          return false;
      }
    }
  }
  cout << "Ran " << testcount << " tests. Your code is probably ok. " << endl;

  return true;
}

typedef struct {
  EWAHBoolArray<uint32_t> gpr[32][4];
} vcpu_ctx;

typedef struct {
  vcpu_ctx vcpu;
  void *uval;
} thread_ctx_t;

bool funnytest() {
  cout << "[funnytest] checking funnytest" << endl;
  thread_ctx_t *tctx = new thread_ctx_t();
  for (int k = 0; k < 10; ++k)
    if (tctx->vcpu.gpr[k][0].get(1)) {
      return false; // bug
    }

  for (int k = 0; k < 10; ++k)
    tctx->vcpu.gpr[k][0].set(1);
  for (int k = 0; k < 10; ++k)
    if (!tctx->vcpu.gpr[k][0].get(1)) {
      return false; // bug
    }

  cout << tctx->vcpu.gpr[0][0] << endl;
  delete tctx;
  cout << "Your code is probably ok. " << endl;
  return true;
}

template <class uword> bool arrayinit() {
  cout << "[arrayinit] checking arrayinit...sizeof(uword)=" << sizeof(uword)
       << endl;
  EWAHBoolArray<uword> gpr[10];
  for (int k = 0; k < 10; ++k)
    gpr[k].set(k);
  for (int k = 0; k < 10; ++k)
    cout << gpr[k] << endl;
  cout << "Your code is probably ok. " << endl;
  return true;
}

template <class uword> bool countstest1() {
  size_t data1[] = {24,   5068, 5113, 5144, 5212, 5281, 5301, 5435, 5498, 5547,
                    5568, 5748, 6010, 6079, 6151, 6245, 6365, 6533, 6566, 6809,
                    6813, 6904, 7046, 7184, 7258, 7302, 7307, 7382, 7424, 7425,
                    7476, 7518, 7609, 7697, 7776, 7809, 7837, 7889, 7898, 7933,
                    8029, 8091, 8279, 8328, 8372, 8391, 8456, 8601, 8612, 8628,
                    8635, 8869, 8886, 8937, 9059, 9067};
  size_t data2[] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
      45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  59,  60,
      61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
      76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
      91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105,
      106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
      121, 122, 123, 124, 125, 126, 127, 128};
  EWAHBoolArray<uword> b1;
  for (size_t k = 0; k < sizeof(data1) / sizeof(size_t); ++k)
    b1.set(data1[k]);
  EWAHBoolArray<uword> b2;
  for (size_t k = 0; k < sizeof(data2) / sizeof(size_t); ++k)
    b2.set(data2[k]);
  assert(b1.logicalandcount(b2) == b1.logicaland(b2).numberOfOnes());
  assert(b1.logicalorcount(b2) == b1.logicalor(b2).numberOfOnes());
  assert(b1.logicalxorcount(b2) == b1.logicalxor(b2).numberOfOnes());
  assert(b1.logicalandnotcount(b2) == b1.logicalandnot(b2).numberOfOnes());
  return true;
}

template <class uword> bool countstest2() {
  size_t data1[] = {
      193,   261,   738,   1484,  1641,  1703,  1858,  2445,  3183,  3385,
      3406,  4028,  4139,  4369,  6267,  6692,  6720,  6818,  7165,  8388,
      8879,  8898,  9866,  10362, 10465, 11578, 11793, 11871, 12334, 12604,
      12814, 13604, 14287, 14699, 14771, 14797, 15041, 15298, 16649, 17620,
      17712, 18215, 18576, 18689, 18929, 18971, 19326, 19387, 19397, 20478,
      20559, 20590, 20749, 21072, 21830, 21937, 22374, 22547, 23552, 23630,
      23727, 23798, 23862, 24770, 25031, 25037, 25669, 25938, 26877, 27972,
      28392, 29049, 29170, 29429, 30019, 30504, 30606, 30854, 31325, 31360,
      31671, 31960, 31984, 32196, 32483, 32935, 33010, 33163, 33341, 33669,
      33822, 34243, 35003, 35334, 36401, 37430, 37472, 37752, 38012, 38374,
      38560, 38814, 39011, 39014, 39846, 40441, 40465, 40911, 41103, 41915,
      42507, 42942, 43083, 43218, 43748, 44102, 44560, 44607, 45404, 45583,
      45587, 46307, 46728, 47239, 47793, 47799, 47913, 48391};
  size_t data2[] = {
      58,    162,   321,   326,   348,   351,   361,   411,   502,   605,
      656,   675,   765,   1036,  1087,  1213,  1321,  1577,  1818,  1957,
      2022,  2290,  2297,  2384,  2397,  2473,  2752,  2805,  2855,  2896,
      2961,  3646,  3721,  3866,  3931,  4000,  4121,  4199,  4594,  4634,
      4697,  4799,  4876,  5098,  5319,  5454,  5610,  5664,  5851,  5873,
      5892,  5986,  6075,  6127,  6270,  6289,  6337,  6354,  6446,  6571,
      6817,  6842,  6882,  7394,  7398,  7539,  7677,  7841,  7982,  8106,
      8475,  8749,  8769,  8793,  8926,  9015,  9133,  9175,  9207,  9373,
      9511,  9663,  9726,  9848,  10154, 10290, 10614, 10675, 10681, 10798,
      10868, 11302, 11367, 11396, 11405, 11562, 11626, 11633, 11644, 11721,
      11846, 11885, 12322, 12519, 12555, 12591, 12594, 12606, 12618, 12667,
      12752, 12820, 13139, 13175, 13252, 13410, 13431, 13534, 13695, 13883,
      13982, 14080, 14117, 14142, 14172, 14275, 14362, 14505};
  EWAHBoolArray<uword> b1;
  for (size_t k = 0; k < sizeof(data1) / sizeof(size_t); ++k)
    b1.set(data1[k]);
  EWAHBoolArray<uword> b2;
  for (size_t k = 0; k < sizeof(data2) / sizeof(size_t); ++k)
    b2.set(data2[k]);
  assert(b1.logicalandcount(b2) == b1.logicaland(b2).numberOfOnes());
  assert(b1.logicalorcount(b2) == b1.logicalor(b2).numberOfOnes());
  assert(b1.logicalxorcount(b2) == b1.logicalxor(b2).numberOfOnes());
  assert(b1.logicalandnotcount(b2) == b1.logicalandnot(b2).numberOfOnes());
  assert(b2.logicalandnotcount(b1) == b2.logicalandnot(b1).numberOfOnes());
  return true;
}

template <class uword> bool countstest3() {
  size_t data1[] = {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
      30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
      45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  59,  60,
      61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,
      76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
      91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104, 105,
      106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
      121, 122, 123, 124, 125, 126, 127, 128};
  size_t data2[] = {
      31,    141,   468,   692,   2000,  2650,  2715,  2995,  3149,  3337,
      3454,  3465,  3848,  3900,  4215,  4388,  4545,  4801,  4911,  4918,
      5358,  6020,  6114,  6143,  6659,  7031,  7258,  7631,  8300,  9299,
      9416,  9471,  9506,  9541,  9918,  10488, 10616, 10617, 10764, 11199,
      11343, 11439, 11455, 11504, 11761, 11901, 12409, 12543, 12619, 12643,
      12972, 13488, 13858, 14073, 14185, 14270, 14508, 14994, 15264, 15492,
      15497, 15536, 15616, 15897, 15918, 16130, 16498, 16573, 16814, 16832,
      17050, 17426, 17443, 17626, 17949, 18122, 18377, 18754, 18810, 18870,
      18965, 19110, 19242, 19449, 19870, 19966, 20012, 20068, 20301, 20602,
      20854, 21328, 22134, 22362, 22370, 22489, 23324, 23353, 24363, 24689,
      24758, 24969, 25104, 25592, 25855, 26111, 26156, 26268, 26372, 26406,
      26537, 26703, 26999, 27113, 27116, 28218, 28256, 28282, 28552, 28625,
      28679, 28907, 29064, 29303, 29320, 29362, 29388, 29407};
  EWAHBoolArray<uword> b1;
  for (size_t k = 0; k < sizeof(data1) / sizeof(size_t); ++k)
    b1.set(data1[k]);
  EWAHBoolArray<uword> b2;
  for (size_t k = 0; k < sizeof(data2) / sizeof(size_t); ++k)
    b2.set(data2[k]);
  assert(b1.logicalandcount(b2) == b1.logicaland(b2).numberOfOnes());
  assert(b1.logicalorcount(b2) == b1.logicalor(b2).numberOfOnes());
  assert(b1.logicalxorcount(b2) == b1.logicalxor(b2).numberOfOnes());
  assert(b1.logicalandnotcount(b2) == b1.logicalandnot(b2).numberOfOnes());
  assert(b2.logicalandnotcount(b1) == b2.logicalandnot(b1).numberOfOnes());
  return true;
}

template <class uword> bool countstest4() {
  size_t data1[] = {
      941063, 941064, 941065, 941066, 941067, 941068, 941069, 941070, 941071,
      941072, 941073, 941074, 941075, 941076, 941077, 941078, 941079, 941080,
      941081, 941082, 941083, 941084, 941085, 941086, 941087, 941088, 941089,
      941090, 941091, 941092, 941093, 941094, 941095, 941096, 941097, 941098,
      941099, 941100, 941101, 941102, 941103, 941104, 941105, 941106, 941107,
      941108, 941109, 941110, 941111, 941112, 941113, 941114, 941115, 941116,
      941117, 941118, 941119, 941120, 941121, 941122, 941123, 941124, 941125,
      941126, 941127, 941128, 941129, 941130, 941131, 941132, 941133, 941134,
      941135, 941136, 941137, 941138, 941139, 941140, 941141, 941142, 941143,
      941144, 941145, 941146, 941147, 941148, 941149, 941150, 941151, 941152,
      941153, 941154, 941155, 941156, 941157, 941158, 941159, 941160, 941161,
      941162, 941163, 941164, 941165, 941166, 941167, 941168, 941169, 941170,
      941171, 941172, 941173, 941174, 941175, 941176, 941177, 941178, 941179,
      941180, 941181, 941182, 941183, 941184, 941185, 941186, 941187, 941188,
      941189, 941190, 941191, 941192, 941193, 941194, 941195, 941196, 941197,
      941198, 941199, 941200, 941201, 941202, 941203, 941204, 941205, 941206,
      941207, 941208, 941209, 941210, 941211, 941212, 941213, 941214, 941215,
      941216, 941217, 941218, 941219, 941220, 941221, 941222, 941223, 941224,
      941225, 941226, 941227, 941228, 941229, 941230, 941231, 941232, 941233,
      941234, 941235, 941236, 941237, 941238, 941239, 941240, 941241, 941242,
      941243, 941244, 941245, 941246, 941247, 941248, 941249, 941250, 941251,
      941252, 941253, 941254, 941255, 941256, 941257, 941258, 941259, 941260,
      941261, 941262, 941263, 941264, 941265, 941266, 941267, 941268, 941269,
      941270, 941271, 941272, 941273, 941274, 941275, 941276, 941277, 941278,
      941279, 941280, 941281, 941282, 941283, 941284, 941285, 941286, 941287,
      941288, 941289, 941290, 941291, 941292, 941293, 941294, 941295, 941296,
      941297, 941298, 941299, 941300, 941301, 941302, 941303, 941304, 941305,
      941306, 941307, 941308, 941309, 941310, 941311, 941312, 941313, 941314,
      941315, 941316, 941317, 941318};
  size_t data2[] = {34850,  43256,  52417,  61592,  70411,  78960,
                    88376,  216599, 225662, 234391, 251420, 258995,
                    312661, 374271, 434864, 444105, 484562, 506410,
                    534808, 540927, 548993, 626059, 669574, 695383,
                    704422, 711412, 719407, 759742, 941141};
  EWAHBoolArray<uword> b1;
  for (size_t k = 0; k < sizeof(data1) / sizeof(size_t); ++k)
    b1.set(data1[k]);
  EWAHBoolArray<uword> b2;
  for (size_t k = 0; k < sizeof(data2) / sizeof(size_t); ++k)
    b2.set(data2[k]);
  assert(b1.logicalandcount(b2) == b1.logicaland(b2).numberOfOnes());
  assert(b1.logicalorcount(b2) == b1.logicalor(b2).numberOfOnes());
  assert(b1.logicalxorcount(b2) == b1.logicalxor(b2).numberOfOnes());
  assert(b1.logicalandnotcount(b2) == b1.logicalandnot(b2).numberOfOnes());
  assert(b2.logicalandnotcount(b1) == b2.logicalandnot(b1).numberOfOnes());
  return true;
}

template <class uword> bool arrayinit2d() {
  cout << "[arrayinit2d] checking arrayinit...sizeof(uword)=" << sizeof(uword)
       << endl;
  EWAHBoolArray<uint32_t> gpr[10][4];
  for (int k = 0; k < 10; ++k)
    for (int l = 0; l < 4; ++l)
      gpr[k][l].set(k);
  for (int k = 0; k < 10; ++k)
    for (int l = 0; l < 4; ++l)
      cout << gpr[k][l] << endl;
  cout << "Your code is probably ok. " << endl;
  return true;
}

int main(int argc, char **argv) {
  std::string path = "tests/data/";
  if (argc > 1) {
    path = argv[1];
    std::cout << " I am expecting the data files in directory " << path
              << std::endl;
  } else {
    std::cout
        << " You did not pass a directory for the data files, defaulting on "
        << path << std::endl;
  }
  int failures = 0;
  std::string failtext = "[GOT FAILURE] ";
  if (!funnytest()) {
    ++failures;
  }
  if (!testAndNotCompactionEWAHBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotCompactionEWAHBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotCompactionEWAHBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEmpty<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEmpty<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEmpty<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSizeInBits<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSizeInBits<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSizeInBits<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testLargeDirty<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testLargeDirty<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testLargeDirty<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!countstest4<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest4<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest4<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest3<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest3<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest3<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest1<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest1<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest1<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest2<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest2<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!countstest3<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testSerialSize<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSerialSize<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSerialSize<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testInEqualityEWAHBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testInEqualityEWAHBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testInEqualityEWAHBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testFastOrAggregate<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testFastOrAggregate<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testFastOrAggregate<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!arrayinit<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!arrayinit<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!arrayinit<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!arrayinit2d<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!arrayinit2d<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!arrayinit2d<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!dataindexingtest<uint16_t>(path)) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!dataindexingtest<uint32_t>(path)) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!dataindexingtest<uint64_t>(path)) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testRealData(path)) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testIntersects<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testIntersects<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testIntersects<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testNanJiang<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testNanJiang<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testNanJiang<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityEWAHBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityEWAHBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityEWAHBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotEWAHBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotEWAHBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotEWAHBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testCardinalityBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testAndNotBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSerialization<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSerialization<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSerialization<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRawSerialization<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRawSerialization<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRawSerialization<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testGet<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testGet<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testGet<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testLucaDeri<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testLucaDeri<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testLucaDeri<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testSetGet<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSetGet<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSetGet<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testPhongTran()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testSTLCompatibility<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSTLCompatibility<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testSTLCompatibility<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testHemeury<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testHemeury<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testHemeury<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testPhongTran2<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testPhongTran2<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testPhongTran2<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRunningLengthWord<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRunningLengthWord<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testRunningLengthWord<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArray<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArray<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArray<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testEWAHBoolArrayLogical<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayLogical<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayLogical<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testEWAHBoolArrayLogical2<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayLogical2<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayLogical2<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testEWAHBoolArrayAppend<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayAppend<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testEWAHBoolArrayAppend<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  if (!testJoergBukowski<uint16_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testJoergBukowski<uint32_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }
  if (!testJoergBukowski<uint64_t>()) {
    std::cout << failtext << __LINE__ << std::endl;
    ++failures;
  }

  tellmeaboutmachine();
  if (failures == 0) {
    cout << "Your code is ok." << endl;
    return EXIT_SUCCESS;
  }
  cout << "Got " << failures << " failed tests!" << endl;
  return EXIT_FAILURE;
}
