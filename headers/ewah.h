#ifndef EWAH_H
#define EWAH_H

#include "boolarray.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <stdexcept>


using namespace std;

// apparently Microsoft defines macros for min / max
#ifndef min
inline uint min(uint x, uint y) {
        return x > y ? y : x;
}
#endif 

#ifndef max
inline uint max(uint x, uint y) {
        return x > y ? x : y;
}
#endif 

/**
 * This is used below in the Enhanced Word Aligned  Hybrid (EWAH)
 */
template <class uword=uword32>
class RunningLengthWord {
	public:
		RunningLengthWord (uword & data) : mydata(data) {
			// these should disappear when the compiler goes over them (?)
			if(sizeof(ushort)!=2) cerr << "bad ushort" << endl;
			if(sizeof(uword)<2) cerr << "bad uword" << endl;
			assert( (largestliteralcount<<(1+runninglengthbits))>>(1+runninglengthbits) ==  largestliteralcount);
			assert(static_cast<uword>(~(static_cast<uword>(1)+ (largestliteralcount<<(1+runninglengthbits)) + (largestrunninglengthcount<<1)))==static_cast<uword>(0));
		}

		RunningLengthWord(const RunningLengthWord & rlw) : mydata(rlw.mydata) {}

		RunningLengthWord& operator=(const RunningLengthWord & rlw) {
			mydata = rlw.mydata;
			return *this;
		}



		bool getRunningBit() const {
			return mydata & static_cast<uword>(1);
		}
		// static call for people who hate objects
		static inline bool getRunningBit(uword  data) {
			return data & static_cast<uword>(1) ;
		}
		/**
		 * how many words should be filled by the running bit (see previous method)
		 */
		uword getRunningLength() const {
			return (mydata >> 1) & largestrunninglengthcount ;
		}
		// static call for people who hate objects
		static inline uword getRunningLength(uword data) {
			return (data >> 1) & largestrunninglengthcount ;
		}
		/**
		 * followed by how many literal words?
		 */
		uword getNumberOfLiteralWords() const {
			return mydata >> (1+runninglengthbits);
		}

		uint size() const {
			return getRunningLength() + getNumberOfLiteralWords();
		}
		// static call for people who hate objects
		static inline uword getNumberOfLiteralWords(uword data) {
			return data >> (1+runninglengthbits);
		}

		void setRunningBit(bool b) {
			if(b) mydata |= static_cast<uword>(1); else mydata &= ~static_cast<uword>(1);
		}
		// static call for people who hate objects
		static inline void setRunningBit(uword & data, bool b) {
			if(b) data |= static_cast<uword>(1); else data &= ~static_cast<uword>(1);
		}
		void discardFirstWords(uint x) {
			assert(x<= size());
			const uword rl ( getRunningLength() );
			if(rl >= x) {
				setRunningLength(rl - x);
				return;
			}
			x -= rl;
			setRunningLength(0);
			// potentially unsafe...
			setNumberOfLiteralWords(getNumberOfLiteralWords() - x);
		}
		void setRunningLength(uword l) {
			mydata |= shiftedlargestrunninglengthcount;
			mydata &= ((static_cast<uword>(l)) << 1) | notshiftedlargestrunninglengthcount;
		}
		// static call for people who hate objects
		static inline void setRunningLength(uword & data, uword l) {
			data |= shiftedlargestrunninglengthcount;
			data &= ((static_cast<uword>(l)) << 1) | notshiftedlargestrunninglengthcount;
		}

		void setNumberOfLiteralWords(uword l) {
			mydata |= notrunninglengthplusrunningbit;
			mydata &= ((static_cast<uword>(l)) << (runninglengthbits +1) ) |runninglengthplusrunningbit;
		}
		// static call for people who hate objects
		static inline void setNumberOfLiteralWords(uword & data, uword l) {
			data |= notrunninglengthplusrunningbit;
			data &= ((static_cast<uword>(l)) << (runninglengthbits +1) ) |runninglengthplusrunningbit;
		}
		static const uint runninglengthbits = sizeof(uword)*4;//16;
		static const uint literalbits = sizeof(uword)*8 - 1 - runninglengthbits;
		static const uword largestliteralcount = (static_cast<uword>(1)<<literalbits) - 1;
		static const uword largestrunninglengthcount = (static_cast<uword>(1)<<runninglengthbits)-1;//static_cast<uword>(0xFFFFUL) ;
		static const uword shiftedlargestrunninglengthcount = largestrunninglengthcount<<1;
		static const uword notshiftedlargestrunninglengthcount = static_cast<uword>(~shiftedlargestrunninglengthcount);
		static const uword runninglengthplusrunningbit = (static_cast<uword>(1)<<(runninglengthbits+1)) - 1;//static_cast<uword>(0x1FFFFUL);
		static const uword notrunninglengthplusrunningbit =static_cast<uword>(~runninglengthplusrunningbit);
		static const uword notlargestrunninglengthcount =static_cast<uword>(~largestrunninglengthcount);

		uword & mydata;
	private:
};


template <class uword=uword32>
class ConstRunningLengthWord {
	public:
		ConstRunningLengthWord (const uword & data) : mydata(data) {
			// these should disappear when the compiler goes over them (?)
			if(sizeof(ushort)!=2) cerr << "bad ushort" << endl;
			if(sizeof(uword)<2) cerr << "bad uword" << endl;
		}

		ConstRunningLengthWord(const ConstRunningLengthWord & rlw) : mydata(rlw.mydata) {}

		bool getRunningBit() const {
			return mydata & static_cast<uword>(1);
		}

		/**
	 	* how many words should be filled by the running bit (see previous method)
	 	*/
		uword getRunningLength() const {
			return (mydata >> 1) & RunningLengthWord<uword>::largestrunninglengthcount ;
		}

		/**
		 * followed by how many literal words?
		 */
		uword getNumberOfLiteralWords() const {
			return mydata >> (1+RunningLengthWord<uword>::runninglengthbits);
		}

		uint size() const {
			return getRunningLength() + getNumberOfLiteralWords();
		}

		const uword & mydata;
	private:
};

template <class uword=uword32>
class BufferedRunningLengthWord {
	public:
		BufferedRunningLengthWord (const uword & data) : RunningBit(data & static_cast<uword>(1)),
		RunningLength((data >> 1) & RunningLengthWord<uword>::largestrunninglengthcount),
		NumberOfLiteralWords(data >> (1+RunningLengthWord<uword>::runninglengthbits))
		{
		}
		BufferedRunningLengthWord (const RunningLengthWord<uword> & p) : RunningBit(p.mydata & static_cast<uword>(1)),
		RunningLength((p.mydata >> 1) & RunningLengthWord<uword>::largestrunninglengthcount),
		NumberOfLiteralWords(p.mydata >> (1+RunningLengthWord<uword>::runninglengthbits))
		{
		}

		void read(const uword & data) {
			RunningBit = data & static_cast<uword>(1);
			RunningLength = (data >> 1) & RunningLengthWord<uword>::largestrunninglengthcount;
			NumberOfLiteralWords = data >> (1+RunningLengthWord<uword>::runninglengthbits);
		}
		bool getRunningBit() const {
			return RunningBit;
		}
		void discardFirstWords(uint x) {
					assert(x<= size());
					if(RunningLength >= x) {
						RunningLength -= x;
						return;
					}
					x -= RunningLength;
					RunningLength = 0;
					// potentially unsafe...
					NumberOfLiteralWords -= x;
		}
		/**
	 	* how many words should be filled by the running bit (see previous method)
	 	*/
		uword getRunningLength() const {
			return RunningLength ;
		}

		/**
		 * followed by how many literal words?
		 */
		uword getNumberOfLiteralWords() const {
			return NumberOfLiteralWords;
		}

		uint size() const {
			return RunningLength + NumberOfLiteralWords;
		}
		bool RunningBit;
		uword RunningLength;
		uword NumberOfLiteralWords;

};
template <class uword>
class EWAHBoolArray;

template <class uword>
class EWAHBoolArraySparseIterator;

template <class uword=uword32>
class EWAHBoolArrayIterator {
	public:
		bool hasNext()  const {
			return pointer < myparent.size();
		}

		uword next() {
			uword returnvalue;
			if(compressedwords < rl) {
				++compressedwords;
		    		if(b)
		    			returnvalue = notzero;
		    		else
		    			returnvalue =  zero;
		  	} else {
		  		assert (literalwords < lw) ;
		  		++literalwords;
		  		++pointer;
		  		assert(pointer <myparent.size());
		  		returnvalue =  myparent[pointer];
		  	}
		  	if((compressedwords == rl) && (literalwords == lw)) {
		  		++pointer;
		  		if(pointer < myparent.size()) readNewRunningLengthWord();
		  	}
		  	return returnvalue;
		}

		EWAHBoolArrayIterator(const EWAHBoolArrayIterator<uword> & other):pointer(other.pointer),
		myparent(other.myparent),
		compressedwords(other.compressedwords),
		literalwords(other.literalwords),
		rl(other.rl),
		lw(other.lw),
		b(other.b){}

		static const uword zero = 0;
		static const uword notzero=static_cast<uword>(~zero);
private:
	EWAHBoolArrayIterator(const vector<uword> & parent) ;
	void readNewRunningLengthWord() ;
	friend class EWAHBoolArray<uword>;
	friend class EWAHBoolArraySparseIterator<uword>;
	uint pointer;
	const vector<uword>  & myparent;
	uword compressedwords;
	uword literalwords;
	uword rl, lw;
	bool b;
};


template <class uword=uword32>
class EWAHBoolArraySparseIterator {
	public:
		bool hasNext() const {
			return i.hasNext();
		}

		uint position() const {
			return mPosition;
		}

		uword next() {
			uword returnvalue;
			if(i.compressedwords < i.rl) {
		    		if(i.b) {
		    			++mPosition;
		    			++i.compressedwords;
		    			returnvalue = EWAHBoolArrayIterator<uword>::notzero;
		    		} else {
		    			mPosition += i.rl;
		    			i.compressedwords = i.rl;
		    			returnvalue = EWAHBoolArrayIterator<uword>::zero;//next();
		    		}
		  	} else {
		  		assert  (i.literalwords < i.lw);
		  		++i.literalwords;
		  		++i.pointer;
		  		++mPosition;
		  		assert(i.pointer <i.myparent.size());
		  		returnvalue =  i.myparent[i.pointer];
		  	}
		  	if((i.compressedwords == i.rl) && (i.literalwords == i.lw)) {
		  		++i.pointer;
		  		if(i.pointer < i.myparent.size()) i.readNewRunningLengthWord();
		  	}
		  	return returnvalue;
		}

		EWAHBoolArraySparseIterator(const EWAHBoolArraySparseIterator<uword> & other):i(other.i),mPosition(other.mPosition) {}

private:
	EWAHBoolArraySparseIterator(const vector<uword> & parent) : i(parent), mPosition(0){}
	EWAHBoolArrayIterator<uword> i;
	uint mPosition;
	friend class EWAHBoolArray<uword>;
};



class BitmapStatistics {
public:
 BitmapStatistics() : totalliteral(0), totalcompressed(0), runningwordmarker(0), maximumofrunningcounterreached(0) {}
	uint getCompressedSize() const  {return totalliteral+ runningwordmarker;}
	uint getUncompressedSize() const  {return totalliteral+ totalcompressed;}
	uint getNumberOfDirtyWords() const  {return totalliteral;}
	uint getNumberOfCleanWords() const  {return totalcompressed;}
	uint getNumberOfMarkers() const  {return runningwordmarker;}
	uint getOverRuns() const {return maximumofrunningcounterreached;}
	uint totalliteral;
	uint totalcompressed;
	uint runningwordmarker;
	uint maximumofrunningcounterreached;
};

template <class uword>
class EWAHBoolArrayRawIterator;

/**
 * This is where compression
 * happens.
 */

template <class uword=uword32>
class EWAHBoolArray {
	public:
		EWAHBoolArray(): buffer(1,0), sizeinbits(0), lastRLW(0) {
		}
		
		void set(uint i) {
       		// must I complete a word?
       		if ( (sizeinbits % (8*sizeof(uword))) != 0) {
               const uint  possiblesizeinbits = (sizeinbits /(8*sizeof(uword)))*(8*sizeof(uword)) + (8*sizeof(uword));
           		if(possiblesizeinbits<i+1) {
               		sizeinbits = possiblesizeinbits;
           		}
       		}
       		addStreamOfEmptyWords(false, (i/(8*sizeof(uword))) - sizeinbits/(8*sizeof(uword)));
       		uint bittoflip = i-(sizeinbits/(8*sizeof(uword)) * (8*sizeof(uword)));
       		// next, we set the bit
       		RunningLengthWord<uword> lastRunningLengthWord(buffer[lastRLW]);
       		if(( lastRunningLengthWord.getNumberOfLiteralWords() == 0) || ((sizeinbits
-1)/(8*sizeof(uword)) < i/(8*sizeof(uword))) ) {
               const uword newdata = static_cast<uword>(1)<<bittoflip;
           	   addLiteralWord(newdata);
       		} else {
           		buffer[buffer.size()-1] |= static_cast<uword>(1)<<bittoflip;
           		// check if we just completed a stream of 1s
           		if(buffer[buffer.size()-1] == ~static_cast<uword>(0))  {
               		// we remove the last dirty word
               		buffer[buffer.size()-1] = 0;
               		buffer.resize(buffer.size()-1);
               		lastRunningLengthWord.setNumberOfLiteralWords(lastRunningLengthWord.getNumberOfLiteralWords()-1);
               		// next we add one clean word
               		addEmptyWord(true);
           		}
       		}
       		sizeinbits = i+1;
   		}
   		
   		void makeSameSize(EWAHBoolArray & a) {
   			if(a.sizeinbits<sizeinbits)
   				a.padWithZeroes(sizeinbits);
   			else if(sizeinbits<a.sizeinbits) 
   				padWithZeroes(a.sizeinbits);
   		}
		
		enum{RESERVEMEMORY=true};// for speed


		// rawlogicaland and rawlogicalor are the default, but sometimes, sparselogicaland might be faster.
		void sparselogicaland( EWAHBoolArray &a, EWAHBoolArray &out) ;
		void rawlogicaland( EWAHBoolArray &a, EWAHBoolArray &container) ;
		void rawlogicalor( EWAHBoolArray &a, EWAHBoolArray &container) ;
		void reset() {
			buffer.clear();
			buffer.push_back(0);
			sizeinbits = 0;
			lastRLW = 0;
		}

		/**
	 	* convenience method.
	 	*
		// returns the number of words added (storage cost increase)
	 	*/
		inline uint add(const uword  newdata, const uint bitsthatmatter = 8*sizeof(uword));
		inline void printout(ostream &o = cout) {
			toBoolArray().printout(o);
		}
		void debugprintout() const;

		inline uint sizeInBits() const{
			return sizeinbits;
		}
		inline void setSizeInBits(const uint size) {
			sizeinbits = size;
		}
		inline uint sizeInBytes() const{
			return buffer.size()*sizeof(uword);
		}



		// same as addEmptyWord, but you can do several in one shot!
		// returns the number of words added (storage cost increase)
		uint addStreamOfEmptyWords(const bool v, const uint number);
		uint addStreamOfDirtyWords(const uword * v, const uint number);

		// make sure the size of the array is totalbits bits by padding with zeroes.
		// returns the number of words added (storage cost increase)
		inline uint padWithZeroes(const uint totalbits);
		uint64 sizeOnDisk() const;
		inline void write(ostream & out, const bool savesizeinbits=true) const;
		inline void writeBuffer(ostream & out) const;
		inline uint bufferSize() const {return buffer.size();}


		inline void read(istream & in, const bool savesizeinbits=true);
		inline void readBuffer(istream & in, const uint buffersize);

		bool operator==(const EWAHBoolArray & x) const;

		bool operator!=(const EWAHBoolArray & x) const;

		bool operator==(const BoolArray<uword> & x) const;

		bool operator!=(const BoolArray<uword> & x) const;

		EWAHBoolArrayIterator<uword> uncompress() const ;
		EWAHBoolArraySparseIterator<uword> sparse_uncompress() const ;

		EWAHBoolArrayRawIterator<uword> raw_iterator() const ;

		void iterator_sanity_check();

		/*
	 	* implementing append requires some thought.
	 	*/
		void append(const EWAHBoolArray & x);

		/**
		 * For research purposes. This computes the number of
		 * dirty words and the number of compressed words.
		 */
		BitmapStatistics computeStatistics() const;

		BoolArray<uword> toBoolArray() const;
		void appendRowIDs(vector<ulong> & out, const ulong offset = 0) const;
		ulong numberOfOnes();
		void swap(EWAHBoolArray & x);
		const vector<uword> & getBuffer() const {return buffer; };
		enum { wordinbits =  sizeof(uword) * 8, sanitychecks = false};


                /**
                 *Please don't copy your bitmaps!
                 **/
		EWAHBoolArray(const EWAHBoolArray& other) :
		buffer(other.buffer),
		sizeinbits(other.sizeinbits),
		lastRLW(other.lastRLW){
                    assert(buffer.size()<=1);// performance assert!
                    //if(buffer.size()>1) {cerr<<buffer.size()<<endl;throw "xxx";}
                }
                // please, never hard-copy this object. Use the swap method if you must.
		EWAHBoolArray & operator=(const EWAHBoolArray & x) {
			buffer = x.buffer;
			sizeinbits = x.sizeinbits;
			lastRLW = x.lastRLW;
                        assert(buffer.size()<=1);// performance assert!
			return *this;
		}

                /**
                 *if you don't care to copy the bitmap (performance-wise), use this!
                 */
                void expensive_copy(const EWAHBoolArray & x) {
                        buffer = x.buffer;
			sizeinbits = x.sizeinbits;
			lastRLW = x.lastRLW;
                }

                void logicalnot(EWAHBoolArray & x) const;
                void inplace_logicalnot();
	private:



		// private because does not increment the size in bits
		// returns the number of words added (storage cost increase)
		inline uint addLiteralWord(const uword  newdata) ;

		// private because does not increment the size in bits
		// returns the number of words added (storage cost increase)
		uint addEmptyWord(const bool v);
		// this second version "might" be faster if you hate OOP.
		// in my tests, it turned out to be slower!
		// private because does not increment the size in bits
		//inline void addEmptyWordStaticCalls(bool v);

		vector<uword> buffer;
		uint sizeinbits;
		uint lastRLW;
};


template <class uword>
void EWAHBoolArray<uword>::inplace_logicalnot()  {
	uint pointer(0);
	while(pointer <buffer.size()) {
		RunningLengthWord<uword> rlw(buffer[pointer]);
		if(rlw.getRunningBit())
			rlw.setRunningBit(false);
		else
			rlw.setRunningBit(true);
		++pointer;
		for(uint k = 0; k<rlw.getNumberOfLiteralWords();++k) {
			buffer[pointer] = ~buffer[pointer];
		    ++pointer;
		}
	}
}


template <class uword>
void EWAHBoolArray<uword>::logicalnot(EWAHBoolArray & x) const {
    x.reset();
	x.buffer.reserve(buffer.size());
    EWAHBoolArrayRawIterator<uword> i = this->raw_iterator();
    while(i.hasNext()) {
        BufferedRunningLengthWord<uword> & rlw = i.next();
        x.addStreamOfEmptyWords(! rlw.getRunningBit(), rlw.getRunningLength());
        const uword * dw = i.dirtyWords();
        for(uint k = 0 ; k <rlw.getNumberOfLiteralWords();++k) {
          x.addLiteralWord(~ dw[k]);
        }
    }
    x.sizeinbits = this->sizeinbits;
}


template <class uword>
uint EWAHBoolArray<uword>::add(const uword  newdata, const uint bitsthatmatter) {
	sizeinbits += bitsthatmatter;
	if(newdata == 0) {
		return addEmptyWord(0);
	} else if (newdata == static_cast<uword>(~0)) {
		return addEmptyWord(1);
	} else {
		return addLiteralWord(newdata);
	}
}


template <class uword>
inline void EWAHBoolArray<uword>::writeBuffer(ostream & out) const {
	out.write(reinterpret_cast<const char *>(& buffer[0]),sizeof(uword)*buffer.size());
}


template <class uword>
inline void EWAHBoolArray<uword>::readBuffer(istream & in, const uint buffersize) {
	buffer.resize(buffersize);
	in.read(reinterpret_cast<char *>(&buffer[0]),sizeof(uword)*buffersize);
}


template <class uword>
void EWAHBoolArray<uword>::write(ostream & out, const bool savesizeinbits) const {
		if(savesizeinbits)out.write(reinterpret_cast<const char *>( & sizeinbits), sizeof(sizeinbits));
		const uint buffersize = buffer.size();
		out.write(reinterpret_cast<const char *>(& buffersize),sizeof(buffersize));
		out.write(reinterpret_cast<const char *>(& buffer[0]),sizeof(uword)*buffersize);
}


template <class uword>
void EWAHBoolArray<uword>::read(istream & in, const bool savesizeinbits) {
		if(savesizeinbits) in.read(reinterpret_cast<char *>(&sizeinbits), sizeof(sizeinbits));
		else sizeinbits = 0;
		uint buffersize(0);
		in.read(reinterpret_cast<char *>(&buffersize), sizeof(buffersize));
		buffer.resize(buffersize);
		in.read(reinterpret_cast<char *>(&buffer[0]),sizeof(uword)*buffersize);
		if(sanitychecks) iterator_sanity_check() ;
}


template <class uword>
uint EWAHBoolArray<uword>::addLiteralWord(const uword  newdata) {
	RunningLengthWord<uword> lastRunningLengthWord(buffer[lastRLW]);
	uword numbersofar = lastRunningLengthWord.getNumberOfLiteralWords();
	if(numbersofar >= RunningLengthWord<uword>::largestliteralcount) {//0x7FFF) {
		buffer.push_back(0);
		lastRLW = buffer.size() - 1;
		RunningLengthWord<uword> lastRunningLengthWord2(buffer[lastRLW]);
		lastRunningLengthWord2.setNumberOfLiteralWords(1);
		buffer.push_back(newdata);
		return 2;
	}
	lastRunningLengthWord.setNumberOfLiteralWords(numbersofar + 1);
	assert(lastRunningLengthWord.getNumberOfLiteralWords()==numbersofar + 1);
	buffer.push_back(newdata);
	return 1;
}




template <class uword>
uint EWAHBoolArray<uword>::padWithZeroes(const uint totalbits) {
	assert(totalbits >= sizeinbits);
	uint missingbits = totalbits - sizeinbits;
	uint wordsadded = addStreamOfEmptyWords(0, missingbits/wordinbits + ((missingbits % wordinbits != 0) ? 1 : 0));
	assert(sizeinbits >= totalbits);
	assert(sizeinbits  <= totalbits + wordinbits);
	sizeinbits = totalbits;
	return wordsadded;
}



/**
 * This is a low-level iterator.
 */

template <class uword=uword32>
class EWAHBoolArrayRawIterator {
	public:
		EWAHBoolArrayRawIterator(const EWAHBoolArray<uword> & p) : pointer(0),
		myparent(&p.getBuffer()), rlw((*myparent)[pointer]) { //RunningLength(0), NumberOfLiteralWords(0), Bit(0) {
			  if(verbose) {
				  cout<<"created a new raw iterator over buffer of size  "<<myparent->size()<<endl;
			  }
		}
		EWAHBoolArrayRawIterator(const EWAHBoolArrayRawIterator & o) : pointer(o.pointer),
		myparent(o.myparent), rlw(o.rlw) {}


		bool hasNext() const {
			if(verbose)cout<<"call to hasNext, pointer is at "<<pointer<< ", parent.size()= "<<myparent->size()<<endl;
			return pointer < myparent->size();
		}

		BufferedRunningLengthWord<uword> & next() {
			  assert(pointer < myparent->size());
			  rlw.read( (*myparent)[pointer]);
			  pointer += rlw.getNumberOfLiteralWords() + 1;
			  if(verbose)cout<<"call to next, pointer moves to "<<pointer<< ", parent.size()= "<<myparent->size()<<endl;
			  return rlw;
		}

		const uword * dirtyWords()  const {
			assert(pointer>0);
			assert(pointer>=rlw.getNumberOfLiteralWords());
			return & (myparent->at(pointer-rlw.getNumberOfLiteralWords()));
		}

		EWAHBoolArrayRawIterator & operator=(const EWAHBoolArrayRawIterator & other) {
			pointer = other.pointer;
			myparent=other.myparent;
			rlw=other.rlw;
			return *this;
		}

		enum{verbose=false};
	uint pointer;
	const vector<uword>  * myparent;
	BufferedRunningLengthWord<uword> rlw;
private:

	EWAHBoolArrayRawIterator();
};






template <class uword>
EWAHBoolArrayIterator<uword> EWAHBoolArray<uword>::uncompress() const {
		return EWAHBoolArrayIterator<uword>(buffer);
}

template <class uword>
EWAHBoolArrayRawIterator<uword> EWAHBoolArray<uword>::raw_iterator() const {
		return EWAHBoolArrayRawIterator<uword>(*this);
}


template <class uword>
EWAHBoolArraySparseIterator<uword> EWAHBoolArray<uword>::sparse_uncompress() const {
		return EWAHBoolArraySparseIterator<uword>(buffer);
}

template <class uword>
bool EWAHBoolArray<uword>::operator==(const EWAHBoolArray & x) const {
			if(sizeinbits != x.sizeinbits) return false;
			if(buffer.size() != x.buffer.size()) return false;
			for(uint k = 0; k < buffer.size(); ++k)
				if(buffer[k] != x.buffer[k]) return false;
			return true;
}

template <class uword>
void EWAHBoolArray<uword>::swap(EWAHBoolArray & x) {
	buffer.swap(x.buffer);
	uint tmp = x.sizeinbits;
	x.sizeinbits = sizeinbits;
	sizeinbits = tmp;
	tmp = x.lastRLW;
	x.lastRLW = lastRLW;
	lastRLW = tmp;
}

template <class uword>
void EWAHBoolArray<uword>::append(const EWAHBoolArray & x) {
	if(sizeinbits % wordinbits == 0) {
		// hoping for the best?
		sizeinbits += x.sizeinbits;
		ConstRunningLengthWord<uword> lRLW(buffer[lastRLW]);
		if( (lRLW.getRunningLength() == 0) && (lRLW.getNumberOfLiteralWords() == 0)) {
			// it could be that the running length word is empty, in such a case,
			// we want to get rid of it!
			assert(lastRLW == buffer.size()-1);
			lastRLW = x.lastRLW + buffer.size()  - 1;
			buffer.resize(buffer.size()-1);
			buffer.insert(buffer.end(),x.buffer.begin(),x.buffer.end());
		} else {
		  lastRLW = x.lastRLW + buffer.size();
		  buffer.insert(buffer.end(),x.buffer.begin(),x.buffer.end());
		}
	} else {
		stringstream ss;
		ss<<"This should really not happen! You are trying to append to a bitmap having a fractional number of words, that is,  "<<static_cast<int>(sizeinbits)<<" bits with a word size in bits of "<<static_cast<int>(wordinbits)<<". ";
		ss<<"Size of the bitmap being appended: "<<x.sizeinbits<<" bits."<<endl;
		throw invalid_argument(ss.str());
	}
}

template <class uword>
EWAHBoolArrayIterator<uword>::EWAHBoolArrayIterator(const vector<uword> & parent) :
	pointer(0),
	myparent(parent),
	compressedwords(0), literalwords(0), rl(0), lw(0), b(0){
	if(pointer <myparent.size()) readNewRunningLengthWord();
}


template <class uword>
void EWAHBoolArrayIterator<uword>::readNewRunningLengthWord() {
	  literalwords = 0;
	  compressedwords = 0;
	  ConstRunningLengthWord<uword> rlw(myparent[pointer]);
	  rl = rlw.getRunningLength();
	  lw = rlw.getNumberOfLiteralWords();
	  b = rlw.getRunningBit();
	  if((rl == 0) && (lw == 0)) {
		  if(pointer < myparent.size() -1) {
			  ++pointer;
			  readNewRunningLengthWord();
		  } else {
		    assert(pointer >= myparent.size()-1);
		    pointer = myparent.size();
		    assert(! hasNext());
		  }
	  }
}

template <class uword>
BoolArray<uword> EWAHBoolArray<uword>::toBoolArray() const {
	BoolArray<uword> ans(sizeinbits);
	EWAHBoolArrayIterator<uword> i = uncompress();
	int counter = 0;
	while(i.hasNext()) {
		ans.setWord(counter++,i.next());
	}
	return ans;
}

template <class uword>
ulong EWAHBoolArray<uword>::numberOfOnes() {
	ulong c (0);
	EWAHBoolArraySparseIterator<uword> i = sparse_uncompress();
	while(i.hasNext()) {
		const uword currentword = i.next();
		for(int k = 0; k < wordinbits; ++k) {
			if ( (currentword & (static_cast<uword>(1) << k)) != 0)
				++c;
		}
	}
	return c;

}




template <class uword>
void EWAHBoolArray<uword>::appendRowIDs(vector<ulong> & out, const ulong offset) const {
	uint pointer(0);
	ulong currentoffset(offset);
	if(RESERVEMEMORY) out.reserve(buffer.size()+64);// trading memory for speed.
	while(pointer <buffer.size()) {
		ConstRunningLengthWord<uword> rlw(buffer[pointer]);
		if(rlw.getRunningBit()) {
			for(ulong x = 0; x<  static_cast<ulong>(rlw.getRunningLength()*wordinbits);++x) {
				out.push_back(currentoffset + x);
			}
		}
		currentoffset+=rlw.getRunningLength()*wordinbits;
		++pointer;
		for(uint k = 0; k<rlw.getNumberOfLiteralWords();++k) {
			const uword currentword = buffer[pointer];
			for(uint k = 0; k < wordinbits; ++k) {
				if ( (currentword & (static_cast<uword>(1) << k)) != 0)
					out.push_back(currentoffset + k);
			}
			currentoffset+=wordinbits;
		    ++pointer;
		}
	}
	if(out.size()>0)
	while(out.back()>=sizeinbits) {
		out.pop_back();
		if(out.size() == 0) break;
	}
}



template <class uword>
bool EWAHBoolArray<uword>::operator!=(const EWAHBoolArray<uword> & x) const {
	return !(*this == x);
}

template <class uword>
bool EWAHBoolArray<uword>::operator==(const BoolArray<uword> & x) const {
	// could be more efficient
	return (this->toBoolArray() == x);
}

template <class uword>
bool EWAHBoolArray<uword>::operator!=(const BoolArray<uword> & x) const {
	// could be more efficient
	return (this->toBoolArray() != x);
}


template <class uword>
uint EWAHBoolArray<uword>::addStreamOfEmptyWords(const bool v, const uint number) {
	if(number == 0) return 0;
	RunningLengthWord<uword> lastRunningLengthWord(buffer[lastRLW]);
	const bool noliteralword = (lastRunningLengthWord.getNumberOfLiteralWords() == 0);
	//firts, if the last running length word is empty, we align it
	// this
	const uword runlen = lastRunningLengthWord.getRunningLength();
	if( ( noliteralword ) && ( runlen == 0 )) {
		lastRunningLengthWord.setRunningBit(v);
	}
	uint wordsadded (0);
	if( ( noliteralword ) && (lastRunningLengthWord.getRunningBit() == v) && (runlen < RunningLengthWord<uword>::largestrunninglengthcount) ) {
		// that's the easy case, we are just continuing
		uint whatwecanadd = min(number, RunningLengthWord<uword>::largestrunninglengthcount-runlen);
		lastRunningLengthWord.setRunningLength(runlen+whatwecanadd);
		sizeinbits += whatwecanadd*wordinbits;
		if(number - whatwecanadd> 0 ) wordsadded += addStreamOfEmptyWords(v, number - whatwecanadd);
	} else {
		buffer.push_back(0);
		++wordsadded;
		lastRLW = buffer.size() - 1;
		RunningLengthWord<uword> lastRunningLengthWord2(buffer[lastRLW]);
		uint whatwecanadd = min(number, RunningLengthWord<uword>::largestrunninglengthcount);
		lastRunningLengthWord2.setRunningBit(v);
		lastRunningLengthWord2.setRunningLength(whatwecanadd);
		sizeinbits += whatwecanadd*wordinbits;
		if(number - whatwecanadd> 0 ) wordsadded += addStreamOfEmptyWords(v, number - whatwecanadd);
	}
	return wordsadded;
}


template <class uword>
uint EWAHBoolArray<uword>::addStreamOfDirtyWords(const uword * v, const uint number) {
	if(number == 0) return 0;
	RunningLengthWord<uword>  lastRunningLengthWord(buffer[lastRLW]);
	const uword NumberOfLiteralWords = lastRunningLengthWord.getNumberOfLiteralWords();
        assert(RunningLengthWord<uword>::largestliteralcount >= NumberOfLiteralWords);
	const uint whatwecanadd = min(number, RunningLengthWord<uword>::largestliteralcount - NumberOfLiteralWords);//0x7FFF-NumberOfLiteralWords);
        assert(NumberOfLiteralWords+whatwecanadd>=NumberOfLiteralWords);
        assert(NumberOfLiteralWords+whatwecanadd<=RunningLengthWord<uword>::largestliteralcount);
        lastRunningLengthWord.setNumberOfLiteralWords(NumberOfLiteralWords+whatwecanadd);
        assert(lastRunningLengthWord.getNumberOfLiteralWords()==NumberOfLiteralWords+whatwecanadd);
	const uint leftovernumber = number -whatwecanadd;
	// add the dirty words...
	// this could be done more efficiently using memcopy....
	const uint oldsize (buffer.size());
	buffer.resize(oldsize+whatwecanadd);
	memcpy(&buffer[oldsize],v,whatwecanadd*sizeof(uword));
	uint wordsadded(whatwecanadd);
	if(leftovernumber>0) {
		//add
		buffer.push_back(0);
                lastRLW=buffer.size() - 1;
		++wordsadded;
		wordsadded+=addStreamOfDirtyWords(v+whatwecanadd, leftovernumber);
	}
        assert(wordsadded >= number);
	return wordsadded;
}



template <class uword>
uint EWAHBoolArray<uword>::addEmptyWord(const bool v) {
	RunningLengthWord<uword> lastRunningLengthWord(buffer[lastRLW]);
	const bool noliteralword = (lastRunningLengthWord.getNumberOfLiteralWords() == 0);
	//firts, if the last running length word is empty, we align it
	// this
	uword runlen = lastRunningLengthWord.getRunningLength();
	if( ( noliteralword ) && ( runlen == 0 )) {
		lastRunningLengthWord.setRunningBit(v);
		assert(lastRunningLengthWord.getRunningBit() == v);
	}
	if( ( noliteralword ) && (lastRunningLengthWord.getRunningBit() == v) && (runlen < RunningLengthWord<uword>::largestrunninglengthcount) ) {
		lastRunningLengthWord.setRunningLength(runlen+1);
		assert(lastRunningLengthWord.getRunningLength() == runlen+1);
		return 0;
	} else {
		// we have to start anew
		buffer.push_back(0);
		lastRLW = buffer.size() - 1;
		RunningLengthWord<uword> lastRunningLengthWord2(buffer[lastRLW]);
		assert(lastRunningLengthWord2.getRunningLength()==0);
		assert(lastRunningLengthWord2.getRunningBit()==0);
		assert(lastRunningLengthWord2.getNumberOfLiteralWords()==0);
		lastRunningLengthWord2.setRunningBit(v);
		assert(lastRunningLengthWord2.getRunningBit() == v);
		lastRunningLengthWord2.setRunningLength(1);
		assert(lastRunningLengthWord2.getRunningLength() == 1);
		assert(lastRunningLengthWord2.getNumberOfLiteralWords()==0);
		return 1;
	}
}



template <class uword>
void EWAHBoolArray<uword>::sparselogicaland(EWAHBoolArray &a, EWAHBoolArray &container)  {
	makeSameSize(a);
	container.reset();
	if(RESERVEMEMORY) container.buffer.reserve(buffer.size()>a.buffer.size()?buffer.size():a.buffer.size());
	assert(sizeInBits() == a.sizeInBits());
	/**
	 * This could possibly be faster if we go around
	 * the uncompress calls.
	 */
	EWAHBoolArraySparseIterator<uword> i = a.sparse_uncompress();
	EWAHBoolArraySparseIterator<uword> j = sparse_uncompress();
	uint pos (0);
	uword x,y;
	bool ibehindj,jbehindi;
	while(i.hasNext() and j.hasNext()) {
			x = i.next();
			y = j.next();
			ibehindj = i.position() < j.position();
			jbehindi = j.position() < i.position();
			while (( ibehindj and i.hasNext()) or (jbehindi and j.hasNext())) {
				if(ibehindj) x = i.next();
				else if(jbehindi) y = j.next();
				ibehindj = i.position() < j.position();
				jbehindi = j.position() < i.position();
			}
			uint nextnonzero = min(i.position(), j.position());
			if(nextnonzero > pos + 1) {
				container.addStreamOfEmptyWords(0, nextnonzero-pos-1);
				pos += nextnonzero-pos-1;
			}
			if(i.position() == j.position()) {
				container.add(x & y);
				++pos;
			}
	}
	container.setSizeInBits(sizeInBits());
	//return answer;
}



template <class uword>
void EWAHBoolArray<uword>::rawlogicalor(EWAHBoolArray &a, EWAHBoolArray &container) {
	makeSameSize(a);
	container.reset();
	if(RESERVEMEMORY) container.buffer.reserve(buffer.size()+a.buffer.size());
	assert(sizeInBits() == a.sizeInBits());
	EWAHBoolArrayRawIterator<uword> i = a.raw_iterator();
	EWAHBoolArrayRawIterator<uword> j = raw_iterator();
	if(!(i.hasNext() and j.hasNext())) {// hopefully this never happens...
		container.setSizeInBits(sizeInBits());
		return;
	}
	// at this point, this should be safe:
	BufferedRunningLengthWord<uword> & rlwi = i.next();
	BufferedRunningLengthWord<uword> & rlwj = j.next();
	//RunningLength;
	while (true) {
		bool i_is_prey (rlwi.size()<rlwj.size());
		BufferedRunningLengthWord<uword> & prey ( i_is_prey ? rlwi: rlwj);
		BufferedRunningLengthWord<uword> & predator (i_is_prey ? rlwj: rlwi);
		if(prey.getRunningBit() == 0) {
                    // we have a stream of 0x00
			const uint predatorrl (predator.getRunningLength());
			const uint preyrl (prey.getRunningLength());
			if(predatorrl >= preyrl) {
			    const uint tobediscarded =  preyrl ;
				container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
			} else {
				const uint tobediscarded =   predatorrl ;
				container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
				const uword * dw_predator (i_is_prey ? j.dirtyWords(): i.dirtyWords());
				container.addStreamOfDirtyWords(dw_predator, preyrl - tobediscarded);
			}
			predator.discardFirstWords(preyrl);
			prey.discardFirstWords(preyrl);
		} else {
                        // we have a stream of 1x11
			const uint preyrl (prey.getRunningLength());
			predator.discardFirstWords(preyrl);
			prey.discardFirstWords(preyrl);
			container.addStreamOfEmptyWords(1, preyrl);
		}
		const uint predatorrl (predator.getRunningLength());
		if(predatorrl>0){
		if(predator.getRunningBit() == 0) {
			const uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
			const uword * dw_prey (i_is_prey ? i.dirtyWords(): j.dirtyWords());
			const uint tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
			container.addStreamOfDirtyWords(dw_prey, tobediscarded);
			predator.discardFirstWords(tobediscarded);
			prey.discardFirstWords(tobediscarded);
                } else {
                    	const uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
			const uint tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
			predator.discardFirstWords(tobediscarded);
			prey.discardFirstWords(tobediscarded);
			container.addStreamOfEmptyWords(1, tobediscarded);
		}
		}
		assert(prey.getRunningLength() ==0);
		// all that is left to do now is to AND the dirty words
		uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
		if(nbre_dirty_prey > 0) {
			assert(predator.getRunningLength() ==0);
			for(uint k = 0; k< nbre_dirty_prey;++k) {
				container.add(i.dirtyWords()[k] | j.dirtyWords()[k]);
			}
			predator.discardFirstWords(nbre_dirty_prey);
		}
		if( i_is_prey ) {
			if(!i.hasNext()) break;
			rlwi = i.next();
		} else {
			if(!j.hasNext()) break;
			rlwj = j.next();
		}
	}
	container.setSizeInBits(sizeInBits());
}


template <class uword>
void EWAHBoolArray<uword>::rawlogicaland(EWAHBoolArray &a, EWAHBoolArray &container)  {
	makeSameSize(a);
	container.reset();
	if(RESERVEMEMORY) container.buffer.reserve(buffer.size()>a.buffer.size()?buffer.size():a.buffer.size());
	assert(sizeInBits() == a.sizeInBits());
	EWAHBoolArrayRawIterator<uword> i = a.raw_iterator();
	EWAHBoolArrayRawIterator<uword> j = raw_iterator();
	if(!(i.hasNext() and j.hasNext())) {// hopefully this never happens...
		container.setSizeInBits(sizeInBits());
		return;
	}
	// at this point, this should be safe:
	BufferedRunningLengthWord<uword> & rlwi = i.next();
	BufferedRunningLengthWord<uword> & rlwj = j.next();
	//RunningLength;
	while (true) {
		bool i_is_prey (rlwi.size()<rlwj.size());
		BufferedRunningLengthWord<uword> & prey ( i_is_prey ? rlwi: rlwj);
		BufferedRunningLengthWord<uword> & predator (i_is_prey ? rlwj: rlwi);
		if(prey.getRunningBit() == 0) {
			const uint preyrl (prey.getRunningLength());
			predator.discardFirstWords(preyrl);
			prey.discardFirstWords(preyrl);
			container.addStreamOfEmptyWords(0, preyrl);
		} else {
			// we have a stream of 1x11
			const uint predatorrl (predator.getRunningLength());
			const uint preyrl (prey.getRunningLength());
			const uint tobediscarded = (predatorrl >= preyrl) ?  preyrl : predatorrl;
			container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
			const uword * dw_predator (i_is_prey ? j.dirtyWords(): i.dirtyWords());
			container.addStreamOfDirtyWords(dw_predator, preyrl - tobediscarded);
			predator.discardFirstWords(preyrl);
			prey.discardFirstWords(preyrl);
		}
		const uint predatorrl (predator.getRunningLength());
		if(predatorrl>0){
		if(predator.getRunningBit() == 0) {
			const uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
			const uint tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
			predator.discardFirstWords(tobediscarded);
			prey.discardFirstWords(tobediscarded);
			container.addStreamOfEmptyWords(0, tobediscarded);
		} else {
			const uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
			const uword * dw_prey (i_is_prey ? i.dirtyWords(): j.dirtyWords());
			const uint tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
			container.addStreamOfDirtyWords(dw_prey, tobediscarded);
			predator.discardFirstWords(tobediscarded);
			prey.discardFirstWords(tobediscarded);
		}
		}
		assert(prey.getRunningLength() ==0);
		// all that is left to do now is to AND the dirty words
		uint nbre_dirty_prey(prey.getNumberOfLiteralWords());
		if(nbre_dirty_prey > 0) {
			assert(predator.getRunningLength() ==0);
			for(uint k = 0; k< nbre_dirty_prey;++k) {
				container.add(i.dirtyWords()[k] & j.dirtyWords()[k]);
			}
			predator.discardFirstWords(nbre_dirty_prey);
		}
		if( i_is_prey ) {
			if(!i.hasNext()) break;
			rlwi = i.next();
		} else {
			if(!j.hasNext()) break;
			rlwj = j.next();
		}
	}
	container.setSizeInBits(sizeInBits());
}




template <class uword>
BitmapStatistics EWAHBoolArray<uword>::computeStatistics() const {
			//uint totalcompressed(0), totalliteral(0);
			BitmapStatistics bs;
			EWAHBoolArrayRawIterator<uword> i = raw_iterator();
			while(i.hasNext()) {
				BufferedRunningLengthWord<uword>  &brlw (i.next());
				++bs.runningwordmarker;
				bs.totalliteral += brlw.getNumberOfLiteralWords();
				bs.totalcompressed += brlw.getRunningLength();
				if(brlw.getRunningLength() == RunningLengthWord<uword>::largestrunninglengthcount) {
					++bs.maximumofrunningcounterreached;
				}
			}
			return bs;
}

template <class uword>
void EWAHBoolArray<uword>::iterator_sanity_check() {
	EWAHBoolArrayIterator<uword> i = uncompress();
	uint64 counter(0);
	while(i.hasNext()) {
		i.next();
		counter += 8*sizeof(uword);
	}
	if(counter<sizeinbits) {
		cerr <<"the uncompressed bitmap has size at most "<<counter<<" bits, yet it is supposed to be of size "<<sizeinbits<<endl;
	}
	if(counter - 8*sizeof(uword)>=sizeinbits) {
		cerr <<"the uncompressed bitmap has size at most "<<counter<<" bits and over "<< (counter - 8*sizeof(uword))<<", yet it is supposed to be of size "<<sizeinbits<<endl;
	}
	assert(counter>=sizeinbits);
	assert(counter - 8*sizeof(uword)<sizeinbits );
}

template <class uword>
void EWAHBoolArray<uword>::debugprintout() const {
			cout << "==printing out EWAHBoolArray=="<<endl;
			cout <<"Number of compressed words: "<< buffer.size()<< endl;
			uint pointer = 0;
			while(pointer <buffer.size()) {
				ConstRunningLengthWord<uword> rlw(buffer[pointer]);
				bool b = rlw.getRunningBit() ;
				uword rl =  rlw.getRunningLength() ;
				uword lw = rlw.getNumberOfLiteralWords();
				cout << "pointer = "<<pointer<<" running bit="<<b<<" running length="<<rl<<" lit. words="<<lw<<endl;
				for(uint j = 0; j < lw ;++j) {
				  const uword & w = buffer[pointer+j+1];
				  cout<<toBinaryString(w)<<endl;;
				}
				pointer += lw + 1;
			}
			cout << "==END=="<<endl;
}

template <class uword>
uint64 EWAHBoolArray<uword>::sizeOnDisk() const {
	return sizeof(sizeinbits)+sizeof(uint)+sizeof(uword)*static_cast<uint64>(buffer.size());
}




#endif
