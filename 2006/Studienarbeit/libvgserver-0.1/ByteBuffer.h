// $Id: ByteBuffer.h 130 2006-05-14 17:11:25Z bingmann $

#ifndef VGS_ByteBuffer_H
#define VGS_ByteBuffer_H

#include <stdlib.h>
#include <assert.h>

#include <string>

namespace VGServer {

/** ByteBuffer is an inline utility class used by the graph server to enclose a
 * continuous memory space and securely return it to the client. The class has
 * two virtual functions, which can be used to implement ByteBuffer
 * derivations, which use other alloc() and free() functions. */

class ByteBuffer
{
protected:
    /// pointer to the allocated memory buffer
    char *_data;

    /// used bytes within the buffer
    size_t _size;

    /// allocated bytes at _data
    size_t _buff;

public:
    /// create an empty object
    explicit inline ByteBuffer() {
	_data = NULL;
	_size = _buff = 0;
    }

    /// create a object with n bytes preallocated
    explicit inline ByteBuffer(size_t n) {
	_data = NULL;
	_size = _buff = 0;
	alloc(n);
    }

    /// copy constructor: create a new duplicate object.
    explicit inline ByteBuffer(const ByteBuffer &o) {
	_data = NULL;
	_size = _buff = 0;
	assign_copy(o.data(), o.size());
    }

    /// assignment: copy memory
    inline ByteBuffer& operator=(const ByteBuffer &o) {
	assign_copy(o.data(), o.size());
	return *this;
    }

    /// destroys the memory space
    virtual ~ByteBuffer() {
	dealloc();
    }

    /// return a const pointer to the currently kept memory
    inline const char *data() const
    { return _data; }

    /// return a pointer to the currently kept memory
    inline char *data()
    { return _data; }

    /// return the currently used length in bytes
    inline size_t size() const
    { return _size; }

    /// return the number of available bytes in the buffer
    inline size_t buffsize() const
    { return _buff; }

    /// returns true if the buffer is empty
    inline bool empty() const
    { return size() == 0; }

    /// set the currently used data
    inline void set_size(size_t s)
    { assert(s <= _buff); _size = s; }

    /// automatic conversion cast to std::string (copies memory)
    inline operator std::string() const
    { return std::string(_data, _size); }

    /// explicit conversion to std::string
    inline std::string toString() const
    { return std::string(_data, _size); }

    /// clears the memory contents. does not deallocate the memory
    inline void clear()
    { _size = 0; }

    /// make sure that at least n bytes are allocated. if less than n bytes are
    /// available, then enlarge the buffer to exactly n bytes, or the next
    /// possible size.
    virtual void alloc(size_t n)
    {
	if (_size < n)
	{
	    _size = _buff = n;
	    _data = static_cast<char*>(realloc(_data, _size));
	}
    }

    /// make sure that at least n bytes are allocated. if less than n bytes are
    /// available, enlarge the buffer. this function may choose to allocate
    /// considerably more than n bytes in expectance of more memory need.
    virtual void grow(size_t n)
    {
	if (_buff < n)
	{
	    // place to adapt the buffer growing algorithm as need.
	    while(_buff < n) {
		if (_buff < 256) _buff = 512;
		else if (_buff < 1024*1024) _buff = 2*_buff;
		else _buff += 1024*1024;
	    }

	    _data = static_cast<char*>(realloc(_data, _buff));
	}
    }

    /// deallocates the kept memory space (we use dealloc() instead of free()
    /// as a name, because sometimes "free" is replaced by the preprocessor)
    inline void dealloc()
    {
	if (_data) free(_data);
	_data = NULL;
	_size = _buff = 0;
    }

    /// detach the memory from the object. returns the memory pointer
    inline const char *detach() {
	const char *data = _data;
	_data = NULL;
	_size = _buff = 0;
	return data;
    }

    /// detach the memory from the object, returning a std::pair containing
    /// memory pointer and size.
    inline std::pair<const char*, size_t> detach_pair()
    {
	std::pair<const char*, size_t> pair(_data, _size);
	_data = NULL;
	_size = 0;
	return pair;
    }

    /// associate this object with a new memory buffer. It will be deallocated
    /// with dealloc()!
    inline void assign_keep(void *data, size_t len)
    {
	dealloc();
	_data = static_cast<char*>(data);
	_size = _buff = len;
    }

    /// copy a memory range into the buffer
    inline void assign_copy(const void *data, size_t len)
    {
	if (_buff < len) alloc(len);

	memcpy(_data, data, len);
	_size = len;
    }
};

} // namespace VGServer

#endif // VGS_ByteBuffer_H
