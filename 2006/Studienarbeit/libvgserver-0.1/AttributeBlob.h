// $Id: AttributeBlob.h 114 2006-04-30 18:01:54Z bingmann $

#ifndef VGS_AttributeBlob_H
#define VGS_AttributeBlob_H

#include <stdlib.h>
#include <assert.h>

#include <utility>
#include <string>

#include "GraphTypes.h"
#include "AnyType.h"
#include "AttributeProperties.h"

namespace VGServer {

/** TpAttributeBlob is the growing byte oriented memory buffer used in the
 * graph server to store attribute values packed in a large array. It provides
 * sequential write-function and random access functions to extract attribute
 * positions. It has one pure virtual function which should be a function
 * defining the grow policy. */

template <typename AllocPolicy>
class TpAttributeBlob
{
private:
    /// our own type
    typedef TpAttributeBlob<AllocPolicy>	thistype;

    /// pointer to the allocated memory buffer
    char *_data;

    /// length of the buffer
    unsigned int _size;

    /// pure virtual function defining the grow policy, we use a virtual
    /// function here instead of a template class, in order to be able to use
    /// the base class AttributeBlob as function parameters.
    //virtual unsigned int realloc_size(unsigned int current_capacity, unsigned int min_new_capacity) = 0;

public:
    /// create a new empty object
    inline TpAttributeBlob()
	: _data(NULL), _size(0)
    {
    }

    /// create an object with n bytes preallocated
    explicit inline TpAttributeBlob(unsigned int n)
	: _data(NULL), _size(0)
    {
	alloc(n);
    }

    /// create an object and copy the specified memory range into it
    explicit inline TpAttributeBlob(const void *data, unsigned int len)
	: _data(NULL), _size(0)
    {
	copy(0, data, len);
    }

    /// copy constructor: allocates a new buffer of the same size.
    inline TpAttributeBlob(const thistype &other)
	: _data(NULL), _size(0)
    {
	copy(0, other.data(), other.size());
    }

    /// assignment operator: copies data of the other into buffer. Does not
    /// zero unused space if our butter happens to be larger.
    inline thistype& operator=(const thistype &other)
    {
	copy(0, other.data(), other.size());
	return *this;
    }

    /// destroys the memory buffer
    inline virtual ~TpAttributeBlob() {
	dealloc();
    }

    /// return a pointer to the currently kept memory
    inline const char *data() const
    { return _data; }

    /// return a pointer to the currently kept memory
    inline char *data()
    { return _data; }

    /// return a pointer to the currently kept memory with the given byte
    /// offset. Doesnt check that it is valid.
    inline const char *data(unsigned int i) const
    { return _data + i; }

    /// return the currently allocated buffer size in bytes. this is _not_ the
    /// length of the contained data.
    inline unsigned int size() const
    { return _size; }

    /// returns true if the TpAttributeBlob is empty
    inline bool empty() const
    { return size() == 0; }

    /// make sure that at least n bytes are allocated
    inline void alloc(unsigned int n)
    { if (_size < n) _alloc(n); }

    /// make sure that at least n bytes are allocated: uses realloc
    void _alloc(unsigned int n)
    {
	if (_size < n)
	{
	    unsigned int newcap = AllocPolicy::realloc_size(_size, n);
	    assert(n <= newcap);

	    _data = static_cast<char*>(realloc(_data, newcap));
	    //printf("TpAttributeBlob::realloc: %u\n", _size);

	    // TODO: remove this debug tool
	    for(unsigned int i = _size; i < newcap; i++)
		_data[i] = i;

	    _size = newcap;
	}
    }

    /// clears the memory contents. does not deallocate the memory
    void clear()
    { _size = 0; }

    /// deallocates the kept memory space (we use dealloc() instead of free()
    /// as a name, because sometimes "free" is replaced by the preprocessor)
    inline void dealloc()
    {
	if (_data) free(_data);
	_data = NULL;
	_size = 0;
    }

    /// detach the memory from the object. returns the memory pointer
    inline const char *detach() {
	const char *data = _data;
	_data = NULL;
	_size = 0;
	return data;
    }

    /// detach the memory from the object, returning a std::pair containing
    /// memory pointer and size.
    inline std::pair<const char*, unsigned int> detach_pair()
    {
	std::pair<const char*, unsigned int> pair(_data, _size);
	_data = NULL;
	_size = 0;
	return pair;
    }

    /// copys a range into a std::string and return it, used by the tests to
    /// check the contents.
    inline std::string getRange(unsigned int i, unsigned int len) const
    { assert(i+len < _size); return std::string(data(i), len); }

    /// exchange the data array with another TpAttributeBlob
    inline void swap(thistype &other)
    {
	std::swap(_data, other._data);
	std::swap(_size, other._size);
    }

    // *** Read Functions

    /// return the character at the given position
    inline char operator[](unsigned int i) const
    {
	assert(i < _size);
	return _data[i];
    }

    /** returns a bit index based at the given position
     *	@param i	index into the buffer
     *  @param bitnum	index of the bit returned, 0 based, can be larger than 8
    */
    inline bool getBool(unsigned int i, unsigned int bitnum) const
    {
	i += bitnum / 8;
	bitnum %= 8;
	assert(i < _size);

	return (_data[i] & (1 << bitnum)) != 0;
    }

    /// function template used to retrieve the value any simple type at a given
    /// index.
    template <typename T>
    inline T get(unsigned int i) const
    {
	assert(i + sizeof(T)-1 < _size);
	return *reinterpret_cast<T*>(_data+i);
    }

    /// returns a (short) string at the given position
    inline std::string getString(unsigned int i) const
    {
	unsigned char slen = get<unsigned char>(i);
	assert(i + sizeof(char)-1 + slen < _size);
	return std::string(_data + i + 1, slen);
    }

    /// returns a long string at the given position
    inline std::string getLongstring(unsigned int i) const
    {
	unsigned int slen = get<unsigned int>(i);
	assert(i + sizeof(int)-1 + slen < _size);
	return std::string(_data + i + 4, slen);
    }

    /// reads a defined AnyType at the position i
    void getAnyType(unsigned int i, AnyType &v) const;

    // *** Write Functions

    /// copys over a block of bytes into the given index position
    inline void copy(unsigned int i, const void *indata, unsigned int inlen)
    {
	alloc(i + inlen);
	memcpy(_data + i, indata, inlen);
    }

    /// zeros a block of bytes at the given index
    inline void zero(unsigned int i, unsigned int len)
    {
	alloc(i + len);
	memset(_data + i, 0, len);
    }

    /// move a block of bytes within the blob. this is used to make room for
    /// new attribute values. Parameter order is _not_ the same as for memmove()!
    inline void move(unsigned int src, unsigned int dst, unsigned int len)
    {
	alloc(dst + len);
	assert(src + len <= _size);
	memmove(_data + dst, _data + src, len);
    }

    /** puts a bit at the given index
     *	@param i	index into the buffer
     *  @param bitnum	index of the bit, 0 based, can be larger than 8
     *  @param v	value to put
    */
    inline void putBool(unsigned int i, unsigned int bitnum, bool v)
    {
	i += bitnum / 8;
	bitnum %= 8;
	alloc(i + 1);

	if (v) {
	    _data[i] |= (1 << bitnum);
	}
	else {
	    _data[i] &= ~(1 << bitnum);
	}
    }

    /// puts the value of the given template type at the given position
    template <typename ValTp>
    inline void put(unsigned int i, ValTp v)
    {
	alloc(i + sizeof(ValTp));
	*reinterpret_cast<ValTp*>(_data+i) = v;
    }

    /// writes a string at the given position
    inline void putString(unsigned int i, const std::string &s)
    {
	alloc(i + 1 + s.size());
	put<unsigned char>(i, static_cast<unsigned char>(s.size()));
	memcpy(_data+i+1, s.data(), s.size());
    }

    /// writes a long string at the given positon
    inline void putLongstring(unsigned int i, const std::string &s)
    {
	alloc(i + 4 + s.size());
	put<unsigned int>(i, static_cast<unsigned int>(s.size()));
	memcpy(_data+i+4, s.data(), s.size());
    }

    /// writes a defined AnyType at the position i
    void putAnyType(unsigned int i, const AnyType &v);

    /// calculate the length of an attribute value chain starting at position chidx.
    unsigned int getAttrChainLength(unsigned int chidx, const AttributePropertiesList &attrlist) const;

    /// retrieve the value of an attribute within the attribute chain starting at position chidx
    class AnyType getAttrChainValue(unsigned int chidx, unsigned int attrid, const AttributePropertiesList &attrlist) const;

    /// write (change) an attribute value within the attribute chain starting at position 0
    bool putAttrChainValue(const AttributePropertiesList &attrlist,
				  unsigned int attrid, const AnyType &value);

};

// *** Three Specializations of the Template Class

/** AttributeBigBlob is a AttributeBlob specialization for large attribute arrays
 * which grow 1 MB at a time */

class AttributeBigBlobPolicy
{
public:
    static unsigned int realloc_size(unsigned int cap, unsigned int minnewcap)
    {
	// place to adapt the buffer growing algorithm as need.
	while(cap < minnewcap)
	{
	    if (cap < 256) cap = 512;
	    else if (cap < 1024*1024) cap = 2*cap;
	    else cap += 1024*1024;
	}
	return cap;
    }
};

typedef TpAttributeBlob<AttributeBigBlobPolicy> AttributeBigBlob;

/** AttributeVertexTinyblob is a TpAttributeBlob specialization for attribute
 * blob containing values of just one vertex object. Using a different type
 * than VertexTinyBlob gives us type checking, so we cant make copy&paste
 * errors which mixup the two attribute arrays. */

class AttributeVertexTinyBlobPolicy
{
public:
    static unsigned int realloc_size(unsigned int cap, unsigned int minnewcap)
    {
	// place to adapt the buffer growing algorithm as need.
	while(cap < minnewcap)
	{
	    if (cap < 8) cap = 8;
	    else if (cap < 1024) cap = 2*cap;
	    else cap += 1024;
	}
	return cap;
    }
};

typedef TpAttributeBlob<AttributeVertexTinyBlobPolicy> AttributeVertexTinyBlob;

/** AttributeEdgeTinyblob is a TpAttributeBlob specialization for attribute
 * blob containing values of just one edge object. Using a different type than
 * VertexTinyBlob gives us type checking, so we cant make copy&paste errors
 * which mixup the two attribute arrays. */

struct AttributeEdgeTinyBlobPolicy
{
    static unsigned int realloc_size(unsigned int cap, unsigned int minnewcap)
    {
	// place to adapt the buffer growing algorithm as need.
	while(cap < minnewcap)
	{
	    if (cap < 8) cap = 8;
	    else if (cap < 1024) cap = 2*cap;
	    else cap += 1024;
	}
	return cap;
    }
};

typedef TpAttributeBlob<AttributeEdgeTinyBlobPolicy> AttributeEdgeTinyBlob;

} // namespace VGServer

#endif // VGS_ByteOutBuffer_H
