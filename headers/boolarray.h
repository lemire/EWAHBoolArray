#ifndef BOOLARRAY_H
#define BOOLARRAY_H

#include <cassert>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iso646.h> // mostly for Microsoft compilers

typedef unsigned short  ushort;
typedef unsigned long  ulong;
typedef unsigned int  uint;
typedef unsigned long long int uint64;
typedef unsigned short  uword16;
typedef unsigned int  uword32;
typedef unsigned long long  uword64;


using namespace std;

/**
 * A dynamic bitset implementation. (without compression)
 */

template <class uword=uword32>
class BoolArray {
	public:
		// how many bits?
		BoolArray(const uint n, const uword initval= 0):buffer(n / wordinbits + (n % wordinbits == 0 ? 0 : 1),initval),sizeinbits(n){	}
		BoolArray():buffer(),sizeinbits(0){}
		BoolArray(const BoolArray & ba) : buffer(ba.buffer),sizeinbits(ba.sizeinbits){}
		void read(istream & in){
			sizeinbits = 0;
			in.read(reinterpret_cast<char *>(&sizeinbits), sizeof(sizeinbits));
			if(verboseIO) cout << "loading "<< sizeinbits << " bits" <<endl;
			buffer.resize(sizeinbits / wordinbits + (sizeinbits % wordinbits == 0 ? 0 : 1));
			if(verboseIO) cout << "using "<< buffer.size() << " words" <<endl;
			in.read(reinterpret_cast<char *>(&buffer[0]),buffer.size()*sizeof(uword));
		}
		void readBuffer(istream & in,const uint size){
			buffer.resize(size);
			in.read(reinterpret_cast<char *>(&buffer[0]),buffer.size()*sizeof(uword));
			sizeinbits = size*sizeof(uword)*8;
		}

		void setSizeInBits(const uint sizeib) {
			sizeinbits = sizeib;
		}


		void write(ostream & out) {
			write(out,sizeinbits);
		}

		void write(ostream & out, const uint numberofbits) const {
			if(verboseIO) cout << "dumping "<< numberofbits << " bits" <<endl;
	   		const uint size = numberofbits/wordinbits + (numberofbits%wordinbits == 0 ? 0: 1);
	   		if(verboseIO) cout << "using "<< size << " words" <<endl;
	   		out.write(reinterpret_cast<const char *>(&numberofbits), sizeof(numberofbits));
	   		out.write(reinterpret_cast<const char *>(&buffer[0]),size*sizeof(uword));
		}

		void writeBuffer(ostream & out, const uint numberofbits) const {
					if(verboseIO) cout << "dumping "<< numberofbits << " bits" <<endl;
			   		const uint size = numberofbits/wordinbits + (numberofbits%wordinbits == 0 ? 0: 1);
			   		if(verboseIO) cout << "using "<< size << " words" <<endl;
			   		out.write(reinterpret_cast<const char *>(&buffer[0]),size*sizeof(uword));
		}

		uint sizeOnDisk() const {
			uint size = sizeinbits/wordinbits + (sizeinbits%wordinbits == 0 ? 0: 1);
			return sizeof(sizeinbits) + size*sizeof(uword);
		}


		BoolArray& operator=(const BoolArray & x) {
			this->buffer = x.buffer;
			return *this;
		}

		bool operator==(const BoolArray & x) const {
			if(sizeinbits != x.sizeinbits) return false;
			assert(buffer.size() == x.buffer.size());
			for(uint k = 0; k < buffer.size(); ++k)
			if(buffer[k] != x.buffer[k]) return false;
			return true;
		}

		bool operator!=(const BoolArray & x) const {
			return ! operator==(x);
		}

		void setWord(const uint pos, const uword val) {
			assert(pos < buffer.size());
			buffer[pos] = val;
		}

		void add(const uword val){
			if(sizeinbits % wordinbits != 0) throw invalid_argument("you probably didn't want to do this");
			sizeinbits += wordinbits;
			buffer.push_back(val);
		}

		uword getWord(const uint pos) const {
			assert(pos < buffer.size());
			return buffer[pos];
		}

		/**
		 * set to true (whether it was already set to true or not)
		 *
		 * TODO this is an expensive (random access) API, you really ought to
		 * prepare a new word and then append it.
		 */
		void set(const uint pos) {
			buffer[pos/wordinbits] |= ( static_cast<uword>(1) << (pos % wordinbits) ) ;
		}

		/**
		 * set to false (whether it was already set to false or not)
		 *
		 * TODO this is an expensive (random access) API, you really ought to
		 * prepare a new word and then append it.
		 */
		void unset(const uint pos) {
			//assert(pos/wordinbits < buffer.size());
			buffer[pos/wordinbits] |= ~( static_cast<uword>(1) << (pos % wordinbits) ) ;
	    	//assert(!get(pos));
		}

		/**
		 * true of false? (set or unset)
		 *
		 * TODO this is an expensive (random access) API, you really ought to
		 * proceed word-by-word
		 */
		bool get(const ulong pos) const {
			assert(pos/wordinbits < buffer.size());
			//cout << buffer[pos/wordinbits]<< " "<<(buffer[pos/wordinbits] & ( static_cast<uword>(1) << (pos % wordinbits) ))<<endl;
			return (buffer[pos/wordinbits] & ( static_cast<uword>(1) << (pos % wordinbits) )) != 0;
		}

		/**
		 * set all bits to 0
		 */
		void reset() {
			memset(&buffer[0],0,sizeof(uword)*buffer.size());
			sizeinbits = 0;
		}

		//inline uint size() {return buffer.size() * sizeof(uint);}
		uint sizeInBits() const {return sizeinbits; }
		~BoolArray() {}

		void logicaland(const BoolArray & ba, BoolArray & out);
		void logicalor(const BoolArray & ba, BoolArray & out);

		void appendRowIDs(vector<ulong> & answer, const ulong offset = 0) const;


		inline void printout(ostream &o = cout) {
			for(uint k = 0; k < sizeinbits; ++k)
				o << get(k) << " ";
			o << endl;
		}

		void append(const BoolArray & a);

		enum { wordinbits =  sizeof(uword) * 8, verboseIO=false};
  	private:
		vector<uword>  buffer;
		uint sizeinbits;
};

template <class uword>
void BoolArray<uword>::append(const BoolArray & a) {
			if(sizeinbits % wordinbits == 0) {
				buffer.insert(buffer.end(),a.buffer.begin(),a.buffer.end());
			} else {
				// we have to work harder?
				throw invalid_argument("I am a lazy bum, please make sure this never happens");
			}
			sizeinbits += a.sizeinbits;
}

#endif
