
#ifndef HEADERS_SIMPLEVECTOR_H_
#define HEADERS_SIMPLEVECTOR_H_

#include <string.h>
#include <stdexcept>

template <class T>
class simplevector {
public:
	simplevector() : array(NULL), s(0), capacity(1) {
		array = new T[capacity];
	}

	simplevector(size_t ms, T val) : array(NULL), s(ms), capacity(ms) {
		array = new T[capacity];
		for(size_t i = 0; i < ms; ++i)
			array[i] = val;
	}

	void swap(simplevector & o) {
		T* tmp = array;
		array = o.array;
		o.array = tmp;

		size_t tmps = s;
		s = o.s;
		o.s = tmps;

		size_t tmpcapacity = capacity;
		capacity = o.capacity;
		o.capacity = tmpcapacity;
	}
	~simplevector() {
		delete[] array;
	}

	void push_back(const T& t) {
		if(s+1>=capacity) {
			reserve((s+1)*2);
		}
		array[s++] = t;
	}
	// must call reserve before
	void append(const T * t, size_t number) {
		for(size_t i = 0; i < number; ++i) array[s+i] = t[i];
		s+= number;
	}

	T& operator[](size_t i) {
		return array[i];
	}
	const T& operator[](size_t i) const {
		return array[i];
	}

	void clear() {
		s = 0;
	}


	void reserve(size_t newsize) {
		if(newsize > capacity) {
		  T* newarray = new T[newsize];
		  memcpy(newarray,array,sizeof(T) * s);
		  capacity = newsize;
		  delete[] array;
		  array = newarray;
		}
	}

	void resize(size_t newsize) {
		reserve(newsize);
		s = newsize;
	}

	size_t size() const {
		return s;
	}

	simplevector& operator=(const simplevector &o) {
		if(capacity < o.capacity)
			reserve(o.capacity);
		memcpy(array,o.array,o.s*sizeof(T));
		s=o.s;
		return *this;
	}

	T* data() {
		return array;
	}


	const T* data() const {
		return array;
	}

	bool operator==(const simplevector &o) {
		if(s != o.s) return false;
		for(size_t i = 0; i < s; ++i)
			if(o.array[i] != array[i]) return false;
		return true;
	}

	simplevector(const simplevector & o) : array(NULL), s(o.s), capacity(o.capacity) {
		array = new T[capacity];
		memcpy(array,o.array,o.s*sizeof(T));
	}

private:

	T * array;

	size_t s;
	size_t capacity;

};




#endif /* HEADERS_SIMPLEVECTOR_H_ */
