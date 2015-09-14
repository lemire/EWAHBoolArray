/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#include <stdlib.h>
#include "ewah.h"

template<class bitmap>
void demo() {
	bitmap bitset1 = bitmap::bitmapOf(9   , 1, 2, 1000, 1001, 1002, 1003, 1007, 1009,
			100000);
	cout << "first bitset : " << bitset1 << endl;
	bitmap bitset2 = bitmap::bitmapOf(5   , 1, 3, 1000, 1007, 100000);
	cout << "second bitset : " << bitset2 << endl;
	bitmap orbitset;
	bitmap andbitset;
	bitmap xorbitset;
	bitset1.logicalor(bitset2, orbitset);
	bitset1.logicaland(bitset2, andbitset);
	bitset1.logicalxor(bitset2, xorbitset);
	// we will display the or
	cout << "logical and: " << andbitset << endl;
	cout << "memory usage of compressed bitset = " << andbitset.sizeInBytes()
			<< " bytes" << endl;
	// we will display the and
	cout << "logical or: " << orbitset << endl;
	cout << "memory usage of compressed bitset = " << orbitset.sizeInBytes()
			<< " bytes" << endl;
	// we will display the xor
	cout << "logical xor: " << xorbitset << endl;
	cout << "memory usage of compressed bitset = " << xorbitset.sizeInBytes()
			<< " bytes" << endl;
	cout << endl;
}


template<class bitmap>
void demoSerialization() {
    stringstream ss;
    bitmap myarray;
    myarray.addWord(234321);// this is not the same as "set(234321)"!!!
    myarray.addWord(0);
    myarray.addWord(0);
    myarray.addWord(999999);
    //
    cout<<"Writing: "<<myarray<<endl;
    myarray.write(ss);
    //
    bitmap lmyarray;
    lmyarray.read(ss);
    cout<<"Read back: "<<lmyarray<<endl;
    //
    if (lmyarray == myarray)
        cout << "serialization works" << endl;
    else
        cout << "serialization does not works" << endl;
}

template<class bitmap>
void smallIntersectBenchmark() {
	// this is the worst case!!!
	bitmap b1;
	bitmap b2;
	// we fill both of them with lots of values, but we
	// do not let them intersect, to make it difficult!
	int N = 10000000;
	for(int i = 0; i < N; ++i) {
	  b1.set(2*i);
		b2.set(2*i+1);
	}
	size_t offset = 2*N+2;
	for(int i = 0; i< 10000 ; ++i) {
		for(int j = 0; j < 1024; ++j )
		  b1.set(offset + j);
		offset += 1024;
		for(int j = 0; j < 1024; ++j )
		  b2.set(offset + j);
		offset += 1024;
	}
	cout <<" First bitmap has " << b1.numberOfOnes() << " set bits" << endl;
	cout <<" Second bitmap has " << b2.numberOfOnes() << " set bits" << endl;
	const clock_t START = clock();
	if(b1.intersects(b2)) cout << "BUG!" << endl;
	cout << "Time to compute evil intersect query : "<< (double)(clock() - START) / CLOCKS_PER_SEC << " s"<<endl;
}


int main(void) {
	cout<<endl;
	cout<<"====uncompressed example===="<<endl;
	cout<<endl;
	demo<BoolArray<uint32_t> >();
	demoSerialization<BoolArray<uint32_t> >();

	cout<<endl;
	cout<<"====compressed example===="<<endl;
	cout<<endl;
	demo<EWAHBoolArray<uint32_t> >();
	demoSerialization<EWAHBoolArray<uint32_t> >();
	smallIntersectBenchmark<EWAHBoolArray<uint32_t> >();
	smallIntersectBenchmark<EWAHBoolArray<uint64_t> >();
	return EXIT_SUCCESS;

}
