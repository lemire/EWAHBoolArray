# Compressed bitset in C++
[![Build Status](https://travis-ci.org/lemire/EWAHBoolArray.png)](https://travis-ci.org/lemire/EWAHBoolArray) [![Build Status](https://img.shields.io/appveyor/ci/lemire/EWAHBoolArray.svg)](https://ci.appveyor.com/project/lemire/EWAHBoolArray)




## What is this?

The class EWAHBoolArray is a compressed bitset data structure.
It supports several word sizes by a template parameter (16-bit, 32-bit, 64-bit).
You should expect the 64-bit word-size to provide better performance, but
higher memory usage, while a 32-bit word-size might compress a bit better,
at the expense of some performance.

The library also provides a basic BoolArray class which can serve as a traditional
bitmap.


## Real-world usage

EWAH is used to accelerate the distributed version control system Git (http://githubengineering.com/counting-objects/). You can find the C port of EWAH written by the Git team at https://github.com/git/git/tree/master/ewah

The Java counterpart of this library (JavaEWAH) is part of Apache Hive and its derivatives (e.g.,  Apache Spark) and Eclipse JGit. It has been used in production systems for many years. It is part of major Linux distributions.


This library is used by database and information retrieval engines
such as Hustle -- A column oriented, embarrassingly distributed relational
event database (see https://github.com/chango/hustle).



## When should you use a bitmap?

Sets are a fundamental abstraction in
software. They can be implemented in various
ways, as hash sets, as trees, and so forth.
In databases and search engines, sets are often an integral
part of indexes. For example, we may need to maintain a set
of all documents or rows  (represented by numerical identifier)
that satisfy some property. Besides adding or removing
elements from the set, we need fast functions
to compute the intersection, the union, the difference between sets, and so on.


To implement a set
of integers, a particularly appealing strategy is the
bitmap (also called bitset or bit vector). Using n bits,
we can represent any set made of the integers from the range
[0,n): it suffices to set the ith bit is set to one if integer i is present in the set.
Commodity processors use words of W=32 or W=64 bits. By combining many such words, we can
support large values of n. Intersections, unions and differences can then be implemented
 as bitwise AND, OR and ANDNOT operations.
More complicated set functions can also be implemented as bitwise operations.

When the bitset approach is applicable, it can be orders of
magnitude faster than other possible implementation of a set (e.g., as a hash set)
while using several times less memory.


## When should you use compressed bitmaps?

An uncompress BitSet can use a lot of memory. For example, if you take a BitSet
and set the bit at position 1,000,000 to true and you have just over 100kB. That's over 100kB
to store the position of one bit. This is wasteful  even if you do not care about memory:
suppose that you need to compute the intersection between this BitSet and another one
that has a bit at position 1,000,001 to true, then you need to go through all these zeroes,
whether you like it or not. That can become very wasteful.

This being said, there are definitively cases where attempting to use compressed bitmaps is wasteful.
For example, if you have a small universe size. E.g., your bitmaps represent sets of integers
from [0,n) where n is small (e.g., n=64 or n=128). If you are able to uncompressed BitSet and
it does not blow up your memory usage,  then compressed bitmaps are probably not useful
to you. In fact, if you do not need compression, then a BitSet offers remarkable speed.
One of the downsides of a compressed bitmap like those provided by EWAHBoolArray is slower random access:
checking whether a bit is set to true in a compressed bitmap takes longer.


## How does EWAH compares with the alternatives?

EWAH is part of a larger family of compressed bitmaps that are run-length-encoded
bitmaps. They identify long runs of 1s or 0s and they represent them with a marker word.
If you have a local mix of 1s and 0, you use an uncompressed word.

There are many formats in this family beside EWAH:

* Oracle's BBC is an obsolete format at this point: though it may provide good compression,
it is likely much slower than more recent alternatives due to excessive branching.
* WAH is a patented variation on BBC that provides better performance.
* Concise is a variation on the patented WAH. It some specific instances, it can compress
much better than WAH (up to 2x better), but it is generally slower.
* EWAH is both free of patent, and it is faster than all the above. On the downside, it
does not compress quite as well. It is faster because it allows some form of "skipping"
over uncompressed words. So though none of these formats are great at random access, EWAH
is better than the alternatives.

There are other alternatives however. For example, the Roaring
format (https://github.com/lemire/RoaringBitmap) is not a run-length-encoded hybrid. It provides faster random access
than even EWAH.


## Licensing

Apache License 2.0.

Update (May 20th, 2013): though by default I use the Apache License 2.0 (which is compatible with GPL 3.0), you can *also* consider this library licensed under GPL 2.0.


## Dependencies

None. (Will work under MacOS, Windows or Linux.)

Compilers tested: clang++, g++, Intel compiler, Microsoft Visual Studio

It works on x64 processors as well as on 32-bit ARM processors. 

Versions 0.5 and above assume that the compiler supports the C++11 standard.

# Usage (Linux and Linux-like systems)

    make
    ./unit
    make example
    ./example

# Usage (Visual Studio under Windows)

To build with at least Visual Studio 2017 directly in the IDE:
- Grab the code from GitHub, e.g., by cloning it using [GitHub Desktop](https://desktop.github.com/).
- Select the ``Visual C++ tools for CMake`` optional component when installing the C++ Development Workload within Visual Studio.
- Within Visual Studio use ``File > Open > Folder...`` to open the CRoaring folder.
- Right click on ``CMakeLists.txt`` in the parent directory within ``Solution Explorer`` and select ``Build`` to build the project.
- For testing, in the Standard toolbar, drop the ``Select Startup Item...`` menu and choose one of the tests. Run the test by pressing the button to the left of the dropdown.


# Quick code sample

```C++
  #include "ewah.h"

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
```


# Example

Please see example.cpp.
For an example with tabular data, please see example2.cpp.

# Further documentation

If you have ``doxygen`` installed, you can type ``make doxygen`` and create a documentation folder that might prove useful.

# Further reading

Please see


* Daniel Lemire, Owen Kaser, Nathan Kurz, Luca Deri, Chris O'Hara, François Saint-Jacques, Gregory Ssi-Yan-Kai, Roaring Bitmaps: Implementation of an Optimized Software Library, Software: Practice and Experience 48 (4), 2018 [arXiv:1709.07821](https://arxiv.org/abs/1709.07821)
* Daniel Lemire, Owen Kaser, Kamel Aouiche, Sorting improves word-aligned bitmap indexes. Data & Knowledge Engineering 69 (1), pages 3-28, 2010. http://arxiv.org/abs/0901.3751
* Owen Kaser and Daniel Lemire, Compressed bitmap indexes: beyond unions and intersections, Software: Practice and Experience 46 (2), 2016. http://arxiv.org/abs/1402.4466

# Ruby wrapper

Josh Ferguson wrote a wrapper for Ruby.
The implementation is packaged and installable as a ruby gem.

You can install it by typing:

        gem install ewah-bitset



## Persistent storage

If you save the bitmap to disk using "write" and then try to read it again with "read" on a different machine, it *may* crash. The file format is specific to the machine you are using. E.g., using two 64-bit Windows PC might work, but if you send the same data to a 32-bit Linux box it may fail.

You can get better persistence with the readBuffer and writeBuffer methods. They will be consistent across machines except for bit endianness: you may need to manually check bit endianness. Moreover, these methods require you to save some metadata (i.e., buffersize and sizeinbits) yourself in some portable way.


For saving in a persistent way:

     size_t sb = mybitmap.sizeInBits(); // save sb somewhere safe, your responsibility!
     size_t bs = mybitmap.bufferSize(); // save bs somewhere safe, your responsibility!
     mybitmap.writeBuffer(out); // this writes the internal buffer of the bitmap


For loading:

     mybitmap.readBuffer(in,bs); // recover the saved internal buffer, you are responsible for endianness
     mybitmap.setSizeInBits(sb); // you have to do this to get a proper bitmap size
