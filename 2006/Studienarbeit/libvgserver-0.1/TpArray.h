// $Id: TpArray.h 182 2006-06-07 17:48:14Z bingmann $

#ifndef VGS_TpArray_H
#define VGS_TpArray_H

#include <stdlib.h>
#include <assert.h>

namespace VGServer {

/** TpArray is a more simple template array than std::vector. Most importantly:
 * it does not grow quadratic (this gets annoying in the range of 512MB vector
 * sizes). The code is based on Sgi's std::vector's. */

template <typename Tp>
class TpArray
{
public:
    // *** STL-like typedefs

    /// the type of this container array, thus our own type
    typedef TpArray<Tp> tparray;

    /// the value we are talking about here
    typedef Tp value_type;

    /// a reference to an object of the value_type
    typedef value_type& reference;

    /// a read-only reference to an object of the value_type
    typedef const value_type& const_reference;

    /// a read-write pointer possibly to an object of the value_type
    typedef value_type* pointer;

    /// a read-only pointer possibly to an object of the value_type
    typedef const value_type const_pointer;

    /// as in STL an iterator is just about the same as a pointer.
    typedef Tp* iterator;

    /// for const iterator returns
    typedef const Tp* const_iterator;

    /// for size values
    typedef unsigned int size_type;

private:
    /// pointer to the beginning of the data
    Tp*		_begin;

    /// and the end of the used data space
    Tp*		_end;

    /// end of allocated data space.
    Tp*		_finish;

public:
    /// initialize an empty array
    inline TpArray()
	: _begin(NULL), _end(NULL), _finish(NULL)
    {
    }

    /// allocate an array and possibly reverse available space
    explicit inline TpArray(size_type _reserve)
	: _begin(NULL), _end(NULL), _finish(NULL)
    {
	if (_reserve) reserve(0);
    }
    
    /// copy constructor: allocates a new array and copies each element
    inline TpArray(const tparray &other)
	: _begin(NULL), _end(NULL), _finish(NULL)
    {
	reserve(other.size());
	iterator i = begin();
	for(const_iterator o = other.begin(); o != other.end(); o++, i++) {
	    // reads: create a new object at the position i within our array by
	    // copy-constructing a Tp object from the other array.
	    new (i) value_type (*o);
	}
	set_size(other.size());
    }

    /// calls the destructor for all contained elements and frees the array
    inline ~TpArray()
    {
	clear();
	if (_begin)
	{
	    free(_begin);
	    _begin = NULL;
	}
    }

    /// assignment operator: destroys all contained objects and creates copies
    /// of those in the other array
    inline tparray& operator= (const tparray& other)
    {
	if (*this != other)
	{
	    clear();
	    reserve(other.size());
	    iterator i = begin();
	    for(const_iterator o = other.begin(); o != other.end(); o++, i++) {
		// reads: create a new object at the position i within our array by
		// copy-constructing a Tp object from the other array.
		new (i) value_type (*o);
	    }
	    set_size(other.size());
	}
	return *this;
    }

    /// return the capacity (available elements before more need to be
    /// allocated)
    inline size_type capacity() const
    { return size_type(_finish - _begin); }
    
    /// return currently used number of elements in the array
    inline size_type size() const
    { return size_type(_end - _begin); }

    /// returns true if the array is empty
    inline bool empty() const
    { return begin() == end(); }

    /// make sure at least n elements are avaiable before more need to be
    /// allocated.
    inline void reserve(size_type n)
    {
	if (capacity() < n)
	{
	    size_type used = size();
	    
	    // calculate a new capacity
	    size_type newcap = capacity();
	    while(newcap < n) {
		if (newcap < 512) newcap = 512;
		else if (newcap < 1024*1024) newcap = 2 * newcap;
		else newcap += 1024*1024;
	    }

	    // reallocate memory and set new pointers
	    _begin = static_cast<Tp*>(realloc(_begin, newcap * sizeof(Tp)));
	    _end = _begin + used;
	    _finish = _begin + newcap;
	}
    }

    /// manually sets the array size after direct loading
    inline void set_size(size_type n)
    { assert(n <= capacity()); _end = _begin + n; }

    /// return a read/write pointer to the first element
    inline iterator begin()
    { return iterator(_begin); }

    /// return a read-only constant pointer to the first element
    inline const_iterator begin() const
    { return const_iterator(_begin); }

    /// return a read/write pointer just past the last valid element
    inline iterator end()
    { return iterator(_end); }

    /// return a read-only constant pointer just past the last valid element
    inline const_iterator end() const
    { return const_iterator(_end); }

    /// clear destroys all objects in the array and resets it's size
    inline void clear()
    {
	for(iterator i = begin(); i != end(); i++) {
	    i->~Tp();
	}
	_end = _begin;
    }

    /// easy and fast subscription access to an element of the array
    inline reference operator[](size_type n)
    { assert(n < size()); return *(begin() + n); }

    /// easy and fast read-only subscription access
    inline const_reference operator[](size_type n) const
    { assert(n < size()); return *(begin() + n); }

    /// return a reference to the last valid object in the array
    inline reference last()
    { assert(size() > 0); return *(end() - 1); }

    /// return a reference to the last valid object in the array
    inline const_reference last() const
    { assert(size() > 0); return *(end() - 1); }

    /// this function constructs a default object at the end of the array,
    /// growing it if necessary and returns a reference to the new object. This
    /// is the main function to be used when building the data. Thus no
    /// temporary objects are created as with STL's push_back(reference).
    inline reference new_back()
    {
	if (capacity() < size() + 1) reserve(size() + 1);

	pointer newobj = new (_end) value_type();
	_end++;
	return *newobj;
    }

    /// this function copy-constructs a new object at the end of the array from
    /// the given object reference. Therefore this method requires a usually
    /// temporary source object.
    inline void push_back(const_reference obj)
    {
	if (capacity() < size() + 1) reserve(size() + 1);

	new (_end) value_type(obj);
	_end++;
    }

    /// swaps this array's contents with the other array object
    inline void swap(tparray &other)
    {
	std::swap(_begin, other._begin);
	std::swap(_end, other._end);
	std::swap(_finish, other._finish);
    }
};

} // namespace VGServer

#endif // VGS_TpArray_H
