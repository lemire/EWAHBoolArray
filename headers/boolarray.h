/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef BOOLARRAY_H
#define BOOLARRAY_H
#include <iso646.h> // mostly for Microsoft compilers
#include <cassert>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>

using namespace std;

/**
 * A dynamic bitset implementation. (without compression).
 */
template<class uword = uint32_t>
class BoolArray {
public:
    BoolArray(const size_t n, const uword initval = 0) :
        buffer(n / wordinbits + (n % wordinbits == 0 ? 0 : 1), initval),
                sizeinbits(n) {
    }

    BoolArray() :
        buffer(), sizeinbits(0) {
    }

    BoolArray(const BoolArray & ba) :
        buffer(ba.buffer), sizeinbits(ba.sizeinbits) {
    }
    void read(istream & in) {
        sizeinbits = 0;
        in.read(reinterpret_cast<char *> (&sizeinbits), sizeof(sizeinbits));
        buffer.resize(
                sizeinbits / wordinbits
                        + (sizeinbits % wordinbits == 0 ? 0 : 1));
        in.read(reinterpret_cast<char *> (&buffer[0]),
                buffer.size() * sizeof(uword));
    }

    void readBuffer(istream & in, const size_t size) {
        buffer.resize(size);
        in.read(reinterpret_cast<char *> (&buffer[0]),
                buffer.size() * sizeof(uword));
        sizeinbits = size * sizeof(uword) * 8;
    }

    void setSizeInBits(const size_t sizeib) {
        sizeinbits = sizeib;
    }

    void write(ostream & out) {
        write(out, sizeinbits);
    }

    void write(ostream & out, const size_t numberofbits) const {
        const size_t size = numberofbits / wordinbits + (numberofbits
                % wordinbits == 0 ? 0 : 1);
        out.write(reinterpret_cast<const char *> (&numberofbits),
                sizeof(numberofbits));
        out.write(reinterpret_cast<const char *> (&buffer[0]),
                size * sizeof(uword));
    }

    void writeBuffer(ostream & out, const size_t numberofbits) const {
        const size_t size = numberofbits / wordinbits + (numberofbits
                % wordinbits == 0 ? 0 : 1);
        out.write(reinterpret_cast<const char *> (&buffer[0]),
                size * sizeof(uword));
    }

    size_t sizeOnDisk() const {
        size_t size = sizeinbits / wordinbits
                + (sizeinbits % wordinbits == 0 ? 0 : 1);
        return sizeof(sizeinbits) + size * sizeof(uword);
    }

    BoolArray& operator=(const BoolArray & x) {
        this->buffer = x.buffer;
        this->sizeinbits = x.sizeinbits;
        return *this;
    }

    bool operator==(const BoolArray & x) const {
        if (sizeinbits != x.sizeinbits)
            return false;
        assert(buffer.size() == x.buffer.size());
        for (size_t k = 0; k < buffer.size(); ++k)
            if (buffer[k] != x.buffer[k])
                return false;
        return true;
    }

    bool operator!=(const BoolArray & x) const {
        return !operator==(x);
    }

    void setWord(const size_t pos, const uword val) {
        assert(pos < buffer.size());
        buffer[pos] = val;
    }

    void add(const uword val) {
        if (sizeinbits % wordinbits != 0)
            throw invalid_argument("you probably didn't want to do this");
        sizeinbits += wordinbits;
        buffer.push_back(val);
    }

    uword getWord(const size_t pos) const {
        assert(pos < buffer.size());
        return buffer[pos];
    }

    /**
     * set to true (whether it was already set to true or not)
     *
     * This is an expensive (random access) API, you really ought to
     * prepare a new word and then append it.
     */
    void set(const size_t pos) {
        buffer[pos / wordinbits] |= (static_cast<uword> (1) << (pos
                % wordinbits));
    }

    /**
     * set to false (whether it was already set to false or not)
     *
     * This is an expensive (random access) API, you really ought to
     * prepare a new word and then append it.
     */
    void unset(const size_t pos) {
        buffer[pos / wordinbits] |= ~(static_cast<uword> (1) << (pos
                % wordinbits));
    }

    /**
     * true of false? (set or unset)
     */
    bool get(const size_t pos) const {
        assert(pos / wordinbits < buffer.size());
        return (buffer[pos / wordinbits] & (static_cast<uword> (1) << (pos
                % wordinbits))) != 0;
    }

    /**
     * set all bits to 0
     */
    void reset() {
        memset(&buffer[0], 0, sizeof(uword) * buffer.size());
        sizeinbits = 0;
    }

    size_t sizeInBits() const {
        return sizeinbits;
    }

    ~BoolArray() {
    }

    void logicaland(const BoolArray & ba, BoolArray & out);

    void logicalor(const BoolArray & ba, BoolArray & out);

    inline void printout(ostream &o = cout) {
        for (size_t k = 0; k < sizeinbits; ++k)
            o << get(k) << " ";
        o << endl;
    }

    void append(const BoolArray & a);

    enum {
        wordinbits = sizeof(uword) * 8
    };

    vector<uint32_t> toArray() const {
		vector<uint32_t> ans;
		for (size_t k = 0; k < buffer.size(); ++k) {
			const uword myword = buffer[k];
			for (uint32_t offset = 0; offset < sizeof(uword) * 8; ++offset) {
				if ((myword >> offset) == 0)
					break;
				offset += numberOfTrailingZeros((myword >> offset));
				ans.push_back(sizeof(uword) * 8 * k + offset);
			}
		}
		return ans;
	}
private:
    vector<uword> buffer;
    size_t sizeinbits;

};

template<class uword>
void BoolArray<uword>::append(const BoolArray & a) {
    if (sizeinbits % wordinbits == 0) {
        buffer.insert(buffer.end(), a.buffer.begin(), a.buffer.end());
    } else {
        throw invalid_argument("Cannot append if parent does not meet boundary");
    }
    sizeinbits += a.sizeinbits;
}

#endif
