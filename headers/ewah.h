#ifndef EWAH_H
#define EWAH_H

#include "boolarray.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <stdexcept>


using namespace std;

/**
* Returns the binary representation of a binary word.
*/
template <class uword>
inline string toBinaryString(const uword w) {
 stringstream s;
 for(uint k = 0; k <sizeof(uword)*8;++k) {
        if(w & (1<<k)) s <<"1"; else s << "0";
 }
 string ans;
 s >> ans;
 return ans;
}


/**
 * For expert users.
 * This class is used to represent a special type of word storing 
 * a run length. It is defined by the Enhanced Word Aligned  Hybrid (EWAH)
 * format. You don't normally need to access this class.
 */
template <class uword=uword32>
class RunningLengthWord {
public:
    RunningLengthWord (uword & data) : mydata(data) { }

    RunningLengthWord(const RunningLengthWord & rlw) : mydata(rlw.mydata) {}

    RunningLengthWord& operator=(const RunningLengthWord & rlw) {
        mydata = rlw.mydata;
        return *this;
    }



    bool getRunningBit() const {
        return mydata & static_cast<uword>(1);
    }
    

    /**
     * how many words should be filled by the running bit 
     */
    static inline bool getRunningBit(uword  data) {
        return data & static_cast<uword>(1) ;
    }
    
    /**
     * how many words should be filled by the running bit 
     */
    uword getRunningLength() const {
        return (mydata >> 1) & largestrunninglengthcount ;
    }
    

    /**
     * followed by how many literal words?
     */
    static inline uword getRunningLength(uword data) {
        return (data >> 1) & largestrunninglengthcount ;
    }
    
    /**
     * followed by how many literal words?
     */
    uword getNumberOfLiteralWords() const {
        return mydata >> (1+runninglengthbits);
    }

    /**
     * Total of getRunningLength() and getNumberOfLiteralWords() 
     */
    uword size() const {
        return getRunningLength() + getNumberOfLiteralWords();
    }
    

    /**
     * followed by how many literal words?
     */
    static inline uword getNumberOfLiteralWords(uword data) {
        return data >> (1+runninglengthbits);
    }


    /**
     * running length of which type of bits
     */
    void setRunningBit(bool b) {
        if(b) mydata |= static_cast<uword>(1);
        else mydata &= ~static_cast<uword>(1);
    }
    
    

    /**
     * running length of which type of bits
     */
    static inline void setRunningBit(uword & data, bool b) {
        if(b) data |= static_cast<uword>(1);
        else data &= ~static_cast<uword>(1);
    }
    
    
    /**
     * running length of which type of bits
     */
    void discardFirstWords(uword x) {
        assert(x<= size());
        const uword rl ( getRunningLength() );
        if(rl >= x) {
            setRunningLength(rl - x);
            return;
        }
        x -= rl;
        setRunningLength(0);
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
    static const uword largestrunninglengthcount = (static_cast<uword>(1)<<runninglengthbits)-1;
    static const uword shiftedlargestrunninglengthcount = largestrunninglengthcount<<1;
    static const uword notshiftedlargestrunninglengthcount = static_cast<uword>(~shiftedlargestrunninglengthcount);
    static const uword runninglengthplusrunningbit = (static_cast<uword>(1)<<(runninglengthbits+1)) - 1;
    static const uword notrunninglengthplusrunningbit =static_cast<uword>(~runninglengthplusrunningbit);
    static const uword notlargestrunninglengthcount =static_cast<uword>(~largestrunninglengthcount);

    uword & mydata;
private:
};


/**
* For expert users.
*/
template <class uword=uword32>
class ConstRunningLengthWord {
public:

    ConstRunningLengthWord () : mydata(0) {
    }

    ConstRunningLengthWord (const uword  data) : mydata(data) {
    }

    ConstRunningLengthWord(const ConstRunningLengthWord & rlw) : mydata(rlw.mydata) {}

    bool getRunningBit() const {
        return mydata & static_cast<uword>(1);
    }

    /**
    * how many words should be filled by the running bit
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

    uword size() const {
        return getRunningLength() + getNumberOfLiteralWords();
    }

    uword  mydata;
};



/**
* For expert users.
*/
template <class uword=uword32>
class BufferedRunningLengthWord {
public:
    BufferedRunningLengthWord (const uword & data) : RunningBit(data & static_cast<uword>(1)),
        RunningLength((data >> 1) & RunningLengthWord<uword>::largestrunninglengthcount),
        NumberOfLiteralWords(data >> (1+RunningLengthWord<uword>::runninglengthbits)) {
    }
    BufferedRunningLengthWord (const RunningLengthWord<uword> & p) : RunningBit(p.mydata & static_cast<uword>(1)),
        RunningLength((p.mydata >> 1) & RunningLengthWord<uword>::largestrunninglengthcount),
        NumberOfLiteralWords(p.mydata >> (1+RunningLengthWord<uword>::runninglengthbits)) {
    }

    void read(const uword & data) {
        RunningBit = data & static_cast<uword>(1);
        RunningLength = (data >> 1) & RunningLengthWord<uword>::largestrunninglengthcount;
        NumberOfLiteralWords = data >> (1+RunningLengthWord<uword>::runninglengthbits);
    }
    bool getRunningBit() const {
        return RunningBit;
    }
    
    void discardFirstWords(uword x) {
        assert(x<= size());
        if(RunningLength >= x) {
            RunningLength -= x;
            return;
        }
        x -= RunningLength;
        RunningLength = 0;
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

    uword size() const {
        return RunningLength + NumberOfLiteralWords;
    }
    bool RunningBit;
    uword RunningLength;
    uword NumberOfLiteralWords;

};

template <class uword>
class EWAHBoolArray;


/**
* For expert users.
*/
template <class uword>
class EWAHBoolArraySparseIterator;


/**
* For expert users.
*/
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
        b(other.b) {}

    static const uword zero = 0;
    static const uword notzero=static_cast<uword>(~zero);
private:
    EWAHBoolArrayIterator(const vector<uword> & parent) ;
    void readNewRunningLengthWord() ;
    friend class EWAHBoolArray<uword>;
    friend class EWAHBoolArraySparseIterator<uword>;
    size_t pointer;
    const vector<uword>  & myparent;
    uword compressedwords;
    uword literalwords;
    uword rl, lw;
    bool b;
};


/**
* For expert users.
*/
template <class uword=uword32>
class EWAHBoolArraySparseIterator {
public:
    bool hasNext() const {
        return i.hasNext();
    }

    size_t position() const {
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
    EWAHBoolArraySparseIterator(const vector<uword> & parent) : i(parent), mPosition(0) {}
    EWAHBoolArrayIterator<uword> i;
    size_t mPosition;
    friend class EWAHBoolArray<uword>;
};


/**
* Used to go through the set bits. Not optimally fast, but convenient.
*/
template <class uword>
class EWAHBoolArraySetBitForwardIterator {
public:
    enum { wordinbits =  sizeof(uword) * 8};

    
    size_t operator*() {
    	return currentrunoffset+offsetofpreviousrun; 
    }
    EWAHBoolArraySetBitForwardIterator & operator++() {
    	++currentrunoffset;
    	advanceToNextSetBit();
    	return *this;
    }
    
    bool operator==(const EWAHBoolArraySetBitForwardIterator<uword> & o) {
    	// if they are both over, return true
    	if((!o.ready) && (!(this->ready)))
    	  return true;
    	return (buffer == o.buffer) && (pointer == o.pointer) && 
    	(offsetofpreviousrun == o.offsetofpreviousrun) && (currentrunoffset == o.currentrunoffset);
    }

    bool operator!=(const EWAHBoolArraySetBitForwardIterator<uword> & o) {
    	// if they are both over, return false
    	if((!o.ready) && (!(this->ready)))
    	  return false;
    	return (buffer != o.buffer) || (pointer != o.pointer) || 
    	(offsetofpreviousrun != o.offsetofpreviousrun) || (currentrunoffset != o.currentrunoffset);
    }
private:

	bool advanceToNextSetBit() {
		if(!ready) return false;
		if (currentrunoffset<rlw.getRunningLength() * wordinbits) {
			if(rlw.getRunningBit())
			  return true;// nothing to do
			currentrunoffset=rlw.getRunningLength() * wordinbits;//skipping
		}
		while(true) {
			const size_t indexoflitword = (currentrunoffset-rlw.getRunningLength() * wordinbits)/wordinbits;
			if(indexoflitword>= rlw.getNumberOfLiteralWords() ) {
				if(advanceToNextRun())
					return advanceToNextSetBit();
				else {
					ready = false;
					return false;
				}
			}
			const uword currentword = buffer[pointer + 1 +  indexoflitword];
			for(uint inwordpointer = (currentrunoffset-rlw.getRunningLength() * wordinbits)%wordinbits; inwordpointer<wordinbits;++inwordpointer,++currentrunoffset) {
				if((currentword & (static_cast<uword>(1) << inwordpointer))!=0)
				  return true;
			}
		}
	}
	
	bool advanceToNextRun() {
		offsetofpreviousrun += currentrunoffset;
		currentrunoffset = 0;
		pointer += 1 + rlw.getNumberOfLiteralWords();
		if(pointer<buffer.size()) {
		  rlw.mydata = buffer[pointer];
		} else {
			return false;
		}  
	}
	

    EWAHBoolArraySetBitForwardIterator(const vector<uword> & parent, size_t startpointer = 0) : buffer(parent), pointer(startpointer), 
    offsetofpreviousrun(0), currentrunoffset(0), rlw(0), ready(true)  {
    	if(pointer<buffer.size()) {
    	  rlw.mydata = buffer[pointer];
    	  advanceToNextSetBit();
    	} else
    	  ready = false;
    }
    const vector<uword> & buffer;
    size_t pointer;
    size_t offsetofpreviousrun;
    size_t currentrunoffset;
    friend class EWAHBoolArray<uword>;
    ConstRunningLengthWord<uword> rlw;
    bool ready;
};


/**
* For experts.
*/
class BitmapStatistics {
public:
    BitmapStatistics() : totalliteral(0), totalcompressed(0), runningwordmarker(0), maximumofrunningcounterreached(0) {}
    size_t getCompressedSize() const  {
        return totalliteral+ runningwordmarker;
    }
    size_t getUncompressedSize() const  {
        return totalliteral+ totalcompressed;
    }
    size_t getNumberOfDirtyWords() const  {
        return totalliteral;
    }
    size_t getNumberOfCleanWords() const  {
        return totalcompressed;
    }
    size_t getNumberOfMarkers() const  {
        return runningwordmarker;
    }
    size_t getOverRuns() const {
        return maximumofrunningcounterreached;
    }
    size_t totalliteral;
    size_t totalcompressed;
    size_t runningwordmarker;
    size_t maximumofrunningcounterreached;
};

template <class uword>
class EWAHBoolArrayRawIterator;

/**
 * This class is a compressed bitmap.
 * This is where compression
 * happens.
 * The underlying data structure is an STL vector.
 */
template <class uword=uword32>
class EWAHBoolArray {
public:
    EWAHBoolArray(): buffer(1,0), sizeinbits(0), lastRLW(0) {
    }

    /**
     * set the ith bit to true (starting at zero).
     * Auto-expands the bitmap.
     * Note that you must set the bits in increasing order:
     * set(1), set(2) is ok; set(2), set(1) is not ok.
     */
    void set(size_t i);

    /**
    * Make sure the two bitmaps have the same size (padding with zeroes
    * if necessary)
    */
    void makeSameSize(EWAHBoolArray & a) {
        if(a.sizeinbits<sizeinbits)
            a.padWithZeroes(sizeinbits);
        else if(sizeinbits<a.sizeinbits)
            padWithZeroes(a.sizeinbits);
    }

    enum {RESERVEMEMORY=true}; // for speed
    
    typedef EWAHBoolArraySetBitForwardIterator<uword> const_iterator;
    
    
    /*
    * Returns an iterator that can be used to access the position of the
    * set bits
    */
    const_iterator begin() const {
    	return EWAHBoolArraySetBitForwardIterator<uword>(buffer);
    }
    
    
    /*
    * Basically a bogus iterator that can be used together with begin()
    * for constructions such as for(EWAHBoolArray<uword>::iterator i = b.begin(); i!=b.end(); ++i) {}
    */
    const_iterator end() const {
    	return EWAHBoolArraySetBitForwardIterator<uword>(buffer,buffer.size());
    }

    /*
    * compute the logical and with another compressed bitmap
    * answer goes into container, though rawlogicaland is the
    * default, sometimes this version is faster.
    */
    void sparselogicaland( EWAHBoolArray &a, EWAHBoolArray &out) ;
    
    /*
    * compute the logical and with another compressed bitmap
    * answer goes into container
    */
    void rawlogicaland( EWAHBoolArray &a, EWAHBoolArray &container) ;

    /*
    * compute the logical and with another compressed bitmap
    * answer goes into container
    */
    void rawlogicalor( EWAHBoolArray &a, EWAHBoolArray &container) ;


    /*
    * compute the logical and with another compressed bitmap
    * answer goes into container
    * (alias for rawlogicaland)
    */
    void logicaland( EWAHBoolArray &a, EWAHBoolArray &container) {
    	rawlogicaland(a,container);
    }

    /*
    * compute the logical and with another compressed bitmap
    * answer goes into container
    * (alias for rawlogicalor)
    */
    void logicalor( EWAHBoolArray &a, EWAHBoolArray &container)  {
    	rawlogicalor(a,container);
    }

	/**
	* clear the content of the bitmap. It does not
	* release the memory.
	*/
    void reset() {
        buffer.clear();
        buffer.push_back(0);
        sizeinbits = 0;
        lastRLW = 0;
    }

    /**
    * convenience method.
    *
    * returns the number of words added (storage cost increase)
    */
    inline size_t add(const uword  newdata, const uint bitsthatmatter = 8*sizeof(uword));
    
    inline void printout(ostream &o = cout) {
        toBoolArray().printout(o);
    }
    
    /**
    * Prints a verbose description of the content of the compressed bitmap.
    */
    void debugprintout() const;

    inline size_t sizeInBits() const {
        return sizeinbits;
    }
    inline void setSizeInBits(const size_t size) {
        sizeinbits = size;
    }
    inline size_t sizeInBytes() const {
        return buffer.size()*sizeof(uword);
    }



    /*
    * same as addEmptyWord, but you can do several in one shot!
    * returns the number of words added (storage cost increase)
    */
    size_t addStreamOfEmptyWords(const bool v, const size_t number);
    
    /*
    * add a stream of dirty words,, returns the number of words added 
    * (storage cost increase)
    */
    size_t addStreamOfDirtyWords(const uword * v, const size_t number);

    /*
    * make sure the size of the array is totalbits bits by padding with zeroes.
    * returns the number of words added (storage cost increase)
    */
    inline size_t padWithZeroes(const size_t totalbits);
    
    /*
    * Compute the size on disk assuming that it was saved using
    * the method "save".
	*/
    size_t sizeOnDisk() const;
    
    
    /**
    * Save this bitmap to a stream. The file format is 
    * | sizeinbits | buffer lenth | buffer content|
    * the sizeinbits part can be omitted if "savesizeinbits=false".
    * Both sizeinbits and buffer length are saved using the size_t data
    * type which is typically a 32-bit unsigned integer for 32-bit CPUs
    * and a 64-bit unsigned integer for 64-bit CPUs.
    * Note that this format is machine-specific. Note also
    * that the word size is not saved. For robust persistent
    * storage, you need to save this extra information elsewhere.
    */
    inline void write(ostream & out, const bool savesizeinbits=true) const;
    
    /**
    * This only writes the buffer content (see write() method.
    * It is for advanced users.
    */
    inline void writeBuffer(ostream & out) const;
    
    /*
    * size (in words) of the underlying STL vector.
    */
    inline size_t bufferSize() const {
        return buffer.size();
    }

	/**
	* this is the counterpart to the write method.
	* if you set savesizeinbits=false, then you are responsible
	* for setting the value fo the attribute sizeinbits (see method setSizeInBits).
	*/
    inline void read(istream & in, const bool savesizeinbits=true);
    
    
    /**
    * read the buffer from a stream, see method writeBuffer.
    * this is for advanced users.
    */
    inline void readBuffer(istream & in, const size_t buffersize);

    bool operator==(const EWAHBoolArray & x) const;

    bool operator!=(const EWAHBoolArray & x) const;

    bool operator==(const BoolArray<uword> & x) const;

    bool operator!=(const BoolArray<uword> & x) const;
    
    /*
    * For expert users.
	*/
    EWAHBoolArrayIterator<uword> uncompress() const ;
    
    /*
    * For expert users.
	*/
    EWAHBoolArraySparseIterator<uword> sparse_uncompress() const ;

    /*
    * For expert users.
	*/
    EWAHBoolArrayRawIterator<uword> raw_iterator() const ;

    /*
    * Appends the content of some other compressed bitmap 
    * at the end of the current bitmap.
    */
    void append(const EWAHBoolArray & x);

    /**
     * For research purposes. This computes the number of
     * dirty words and the number of compressed words.
     */
    BitmapStatistics computeStatistics() const;

    BoolArray<uword> toBoolArray() const;
    
    /**
    * Convert to a list of positions of "set" bits.
    * The recommender container is vector<size_t>.
    */
    template <class container>
    void appendRowIDs(container & out, const size_t offset = 0) const;
    
    
    /**
    * Convert to a list of positions of "set" bits.
    * The recommender container is vector<size_t>.
    * (alias for appendRowIDs).
    */
    template <class container>
    void appendSetBits(container & out, const size_t offset = 0) const {
    	return appendRowIDs(out,offset);
    }
    
    size_t numberOfOnes();
    
    void swap(EWAHBoolArray & x);
    
    const vector<uword> & getBuffer() const {
        return buffer;
    };
    enum { wordinbits =  sizeof(uword) * 8};


    /**
     *Please don't copy your bitmaps!
     **/
    EWAHBoolArray(const EWAHBoolArray& other) :
        buffer(other.buffer),
        sizeinbits(other.sizeinbits),
        lastRLW(other.lastRLW) {
        assert(buffer.size()<=1);// performance assert!

    }
    
    /*
    * please, never hard-copy this object. Use the swap method if you must.
    */
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
    inline size_t addLiteralWord(const uword  newdata) ;

    // private because does not increment the size in bits
    // returns the number of words added (storage cost increase)
    size_t addEmptyWord(const bool v);
    // this second version "might" be faster if you hate OOP.
    // in my tests, it turned out to be slower!
    // private because does not increment the size in bits
    //inline void addEmptyWordStaticCalls(bool v);

    vector<uword> buffer;
    size_t sizeinbits;
    size_t lastRLW;
};

template <class uword>
void EWAHBoolArray<uword>::set(size_t i) {
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



template <class uword>
void EWAHBoolArray<uword>::inplace_logicalnot()  {
    size_t pointer(0);
    while(pointer <buffer.size()) {
        RunningLengthWord<uword> rlw(buffer[pointer]);
        if(rlw.getRunningBit())
            rlw.setRunningBit(false);
        else
            rlw.setRunningBit(true);
        ++pointer;
        for(size_t k = 0; k<rlw.getNumberOfLiteralWords(); ++k) {
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
        for(size_t k = 0 ; k <rlw.getNumberOfLiteralWords(); ++k) {
            x.addLiteralWord(~ dw[k]);
        }
    }
    x.sizeinbits = this->sizeinbits;
}


template <class uword>
size_t EWAHBoolArray<uword>::add(const uword  newdata, const uint bitsthatmatter) {
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
inline void EWAHBoolArray<uword>::readBuffer(istream & in, const size_t buffersize) {
    buffer.resize(buffersize);
    in.read(reinterpret_cast<char *>(&buffer[0]),sizeof(uword)*buffersize);
}


template <class uword>
void EWAHBoolArray<uword>::write(ostream & out, const bool savesizeinbits) const {
    if(savesizeinbits)out.write(reinterpret_cast<const char *>( & sizeinbits), sizeof(sizeinbits));
    const size_t buffersize = buffer.size();
    out.write(reinterpret_cast<const char *>(& buffersize),sizeof(buffersize));
    out.write(reinterpret_cast<const char *>(& buffer[0]),sizeof(uword)*buffersize);
}


template <class uword>
void EWAHBoolArray<uword>::read(istream & in, const bool savesizeinbits) {
    if(savesizeinbits) in.read(reinterpret_cast<char *>(&sizeinbits), sizeof(sizeinbits));
    else sizeinbits = 0;
    size_t buffersize(0);
    in.read(reinterpret_cast<char *>(&buffersize), sizeof(buffersize));
    buffer.resize(buffersize);
    in.read(reinterpret_cast<char *>(&buffer[0]),sizeof(uword)*buffersize);
}


template <class uword>
size_t EWAHBoolArray<uword>::addLiteralWord(const uword  newdata) {
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
size_t EWAHBoolArray<uword>::padWithZeroes(const size_t totalbits) {
    assert(totalbits >= sizeinbits);
    size_t missingbits = totalbits - sizeinbits;
    size_t wordsadded = addStreamOfEmptyWords(0,  missingbits/wordinbits + ((missingbits % wordinbits != 0) ? 1 : 0));
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

    enum {verbose=false};
    size_t pointer;
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
    for(size_t k = 0; k < buffer.size(); ++k)
        if(buffer[k] != x.buffer[k]) return false;
    return true;
}

template <class uword>
void EWAHBoolArray<uword>::swap(EWAHBoolArray & x) {
    buffer.swap(x.buffer);
    size_t tmp = x.sizeinbits;
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
    compressedwords(0), literalwords(0), rl(0), lw(0), b(0) {
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
size_t EWAHBoolArray<uword>::numberOfOnes() {
    size_t c (0);
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
template <class container>
void EWAHBoolArray<uword>::appendRowIDs(container & out, const size_t offset) const {
    size_t pointer(0);
    size_t currentoffset(offset);
    if(RESERVEMEMORY) out.reserve(buffer.size()+64);// trading memory for speed.
    while(pointer <buffer.size()) {
        ConstRunningLengthWord<uword> rlw(buffer[pointer]);
        if(rlw.getRunningBit()) {
            for(size_t x = 0; x<  static_cast<size_t>(rlw.getRunningLength()*wordinbits); ++x) {
                out.push_back(currentoffset + x);
            }
        }
        currentoffset+=rlw.getRunningLength()*wordinbits;
        ++pointer;
        for(uword k = 0; k<rlw.getNumberOfLiteralWords(); ++k) {
            const uword currentword = buffer[pointer];
            for(uint k = 0; k < wordinbits; ++k) {
                if ( (currentword & (static_cast<uword>(1) << k)) != 0) 
                    out.push_back(currentoffset + k);
            }
            currentoffset+=wordinbits;
            ++pointer;
        }
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
size_t EWAHBoolArray<uword>::addStreamOfEmptyWords(const bool v, const size_t number) {
    if(number == 0) return 0;
    RunningLengthWord<uword> lastRunningLengthWord(buffer[lastRLW]);
    const bool noliteralword = (lastRunningLengthWord.getNumberOfLiteralWords() == 0);
    //firts, if the last running length word is empty, we align it
    // this
    const uword runlen = lastRunningLengthWord.getRunningLength();
    if( ( noliteralword ) && ( runlen == 0 )) {
        lastRunningLengthWord.setRunningBit(v);
    }
    size_t wordsadded (0);
    if( ( noliteralword ) && (lastRunningLengthWord.getRunningBit() == v) && (runlen < RunningLengthWord<uword>::largestrunninglengthcount) ) {
        // that's the easy case, we are just continuing
        uword whatwecanadd = static_cast<uword>( number < static_cast<uword>(RunningLengthWord<uword>::largestrunninglengthcount-runlen) ? number : static_cast<size_t>(RunningLengthWord<uword>::largestrunninglengthcount-runlen) );
        lastRunningLengthWord.setRunningLength(runlen+whatwecanadd);
        sizeinbits += whatwecanadd*wordinbits;
        if(number - whatwecanadd> 0 ) wordsadded += addStreamOfEmptyWords(v, number - whatwecanadd);
    } else {
        buffer.push_back(0);
        ++wordsadded;
        lastRLW = buffer.size() - 1;
        RunningLengthWord<uword> lastRunningLengthWord2(buffer[lastRLW]);
        uword whatwecanadd = static_cast<uword>( number < RunningLengthWord<uword>::largestrunninglengthcount ? number : static_cast<size_t>(RunningLengthWord<uword>::largestrunninglengthcount) );
        lastRunningLengthWord2.setRunningBit(v);
        lastRunningLengthWord2.setRunningLength(whatwecanadd);
        sizeinbits += whatwecanadd*wordinbits;
        if(number - whatwecanadd> 0 ) wordsadded += addStreamOfEmptyWords(v, number - whatwecanadd);
    }
    return wordsadded;
}


template <class uword>
size_t EWAHBoolArray<uword>::addStreamOfDirtyWords(const uword * v, const size_t number) {
    if(number == 0) return 0;
    RunningLengthWord<uword>  lastRunningLengthWord(buffer[lastRLW]);
    const uword NumberOfLiteralWords = lastRunningLengthWord.getNumberOfLiteralWords();
    assert(RunningLengthWord<uword>::largestliteralcount >= NumberOfLiteralWords);
    const size_t whatwecanadd = number < static_cast<uword>(RunningLengthWord<uword>::largestliteralcount - NumberOfLiteralWords) ? number : static_cast<size_t>(RunningLengthWord<uword>::largestliteralcount - NumberOfLiteralWords);//0x7FFF-NumberOfLiteralWords);
    assert(NumberOfLiteralWords+whatwecanadd>=NumberOfLiteralWords);
    assert(NumberOfLiteralWords+whatwecanadd<=RunningLengthWord<uword>::largestliteralcount);
    lastRunningLengthWord.setNumberOfLiteralWords(static_cast<uword>(NumberOfLiteralWords+whatwecanadd));
    assert(lastRunningLengthWord.getNumberOfLiteralWords()==NumberOfLiteralWords+whatwecanadd);
    const size_t leftovernumber = number - whatwecanadd;
    // add the dirty words...
    const size_t oldsize (buffer.size());
    buffer.resize(oldsize+whatwecanadd);
    memcpy(&buffer[oldsize],v,whatwecanadd*sizeof(uword));
    size_t wordsadded(whatwecanadd);
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
size_t EWAHBoolArray<uword>::addEmptyWord(const bool v) {
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
    size_t pos (0);
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
        size_t nextnonzero = i.position()< j.position() ?i.position(): j.position() ;
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
            const uword predatorrl (predator.getRunningLength());
            const uword preyrl (prey.getRunningLength());
            if(predatorrl >= preyrl) {
                const uword tobediscarded =  preyrl ;
                container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
            } else {
                const uword tobediscarded =   predatorrl ;
                container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
                const uword * dw_predator (i_is_prey ? j.dirtyWords(): i.dirtyWords());
                container.addStreamOfDirtyWords(dw_predator, preyrl - tobediscarded);
            }
            predator.discardFirstWords(preyrl);
            prey.discardFirstWords(preyrl);
        } else {
            // we have a stream of 1x11
            const uword preyrl (prey.getRunningLength());
            predator.discardFirstWords(preyrl);
            prey.discardFirstWords(preyrl);
            container.addStreamOfEmptyWords(1, preyrl);
        }
        const uword predatorrl (predator.getRunningLength());
        if(predatorrl>0) {
            if(predator.getRunningBit() == 0) {
                const uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
                const uword * dw_prey (i_is_prey ? i.dirtyWords(): j.dirtyWords());
                const uword tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
                container.addStreamOfDirtyWords(dw_prey, tobediscarded);
                predator.discardFirstWords(tobediscarded);
                prey.discardFirstWords(tobediscarded);
            } else {
                const uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
                const uword tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
                predator.discardFirstWords(tobediscarded);
                prey.discardFirstWords(tobediscarded);
                container.addStreamOfEmptyWords(1, tobediscarded);
            }
        }
        assert(prey.getRunningLength() ==0);
        // all that is left to do now is to AND the dirty words
        uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
        if(nbre_dirty_prey > 0) {
            assert(predator.getRunningLength() ==0);
            for(uword k = 0; k< nbre_dirty_prey; ++k) {
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
            const uword preyrl (prey.getRunningLength());
            predator.discardFirstWords(preyrl);
            prey.discardFirstWords(preyrl);
            container.addStreamOfEmptyWords(0, preyrl);
        } else {
            // we have a stream of 1x11
            const uword predatorrl (predator.getRunningLength());
            const uword preyrl (prey.getRunningLength());
            const uword tobediscarded = (predatorrl >= preyrl) ?  preyrl : predatorrl;
            container.addStreamOfEmptyWords(predator.getRunningBit(), tobediscarded);
            const uword * dw_predator (i_is_prey ? j.dirtyWords(): i.dirtyWords());
            container.addStreamOfDirtyWords(dw_predator, preyrl - tobediscarded);
            predator.discardFirstWords(preyrl);
            prey.discardFirstWords(preyrl);
        }
        const uword predatorrl (predator.getRunningLength());
        if(predatorrl>0) {
            if(predator.getRunningBit() == 0) {
                const uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
                const uword tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
                predator.discardFirstWords(tobediscarded);
                prey.discardFirstWords(tobediscarded);
                container.addStreamOfEmptyWords(0, tobediscarded);
            } else {
                const uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
                const uword * dw_prey (i_is_prey ? i.dirtyWords(): j.dirtyWords());
                const uword tobediscarded = (predatorrl >= nbre_dirty_prey) ? nbre_dirty_prey : predatorrl;
                container.addStreamOfDirtyWords(dw_prey, tobediscarded);
                predator.discardFirstWords(tobediscarded);
                prey.discardFirstWords(tobediscarded);
            }
        }
        assert(prey.getRunningLength() ==0);
        // all that is left to do now is to AND the dirty words
        uword nbre_dirty_prey(prey.getNumberOfLiteralWords());
        if(nbre_dirty_prey > 0) {
            assert(predator.getRunningLength() ==0);
            for(uword k = 0; k< nbre_dirty_prey; ++k) {
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
void EWAHBoolArray<uword>::debugprintout() const {
    cout << "==printing out EWAHBoolArray=="<<endl;
    cout <<"Number of compressed words: "<< buffer.size()<< endl;
    size_t pointer = 0;
    while(pointer <buffer.size()) {
        ConstRunningLengthWord<uword> rlw(buffer[pointer]);
        bool b = rlw.getRunningBit() ;
        uword rl =  rlw.getRunningLength() ;
        uword lw = rlw.getNumberOfLiteralWords();
        cout << "pointer = "<<pointer<<" running bit="<<b<<" running length="<<rl<<" lit. words="<<lw<<endl;
        for(uword j = 0; j < lw ; ++j) {
            const uword & w = buffer[pointer+j+1];
            cout<<toBinaryString(w)<<endl;;
        }
        pointer += lw + 1;
    }
    cout << "==END=="<<endl;
}

template <class uword>
size_t EWAHBoolArray<uword>::sizeOnDisk() const {
    return sizeof(sizeinbits)+sizeof(size_t)+sizeof(uword)*buffer.size();
}




#endif
