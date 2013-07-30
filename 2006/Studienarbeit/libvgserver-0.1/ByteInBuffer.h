// $Id: ByteInBuffer.h 163 2006-05-28 08:59:43Z bingmann $

#ifndef VGS_ByteInBuffer_H
#define VGS_ByteInBuffer_H

#include <stdlib.h>
#include <assert.h>

#include <utility>
#include <string>
#include <stdexcept>

#include "ByteBuffer.h"

namespace VGServer {

/** ByteInBuffer is a cursor class to ByteBuffer implementing extraction
 * functions used in the graph client parser to securely access the byte blob
 * returned by the server. */

class ByteInBuffer
{
private:
    /// reference to read ByteBuffer
    const ByteBuffer	&buff;

    /// current read cursor
    size_t _curr;

    /// used to check the size of elementary types
    inline void check_sizeof() {
	assert( sizeof(char) == 1 );
	assert( sizeof(short) == 2 );
	assert( sizeof(int) == 4 );
    }

public:
    /// create a new cursor object
    inline ByteInBuffer(const ByteBuffer &_buff)
	: buff(_buff), _curr(0)
    {
	check_sizeof();
    }

    /// copy constructor: create a new duplicate object.
    inline ByteInBuffer(const ByteInBuffer &o)
	: buff(o.buff), _curr(o._curr)
    {
    }

    // *** Cursor-Driven Read Functions ***

    /// reset the cursor
    inline void rewind()
    { _curr = 0; }

    /// return the current cursor position
    inline size_t cursor() const
    { return _curr; }

    /// return the number of bytes remaining after the cursor
    inline size_t remaining() const
    { return (_curr < buff.size()) ? (buff.size() - _curr) : 0; }

    /// check that n bytes are available after the cursor
    inline bool cursor_available(unsigned int n) const
    { return (_curr + n <= buff.size()); }

    /// throws a std::underflow_error unless n bytes are available at the cursor
    inline void check_available(unsigned int n) const
    { if (!cursor_available(n)) throw(std::underflow_error("ByteInBuffer underrun")); }

    /// fetch a single item of the template type Tp from the buffer, advancing
    /// the cursor. Be careful with implicit type conversions!
    template <typename Tp>
    inline Tp fetch()
    {
	check_available(sizeof(Tp));

	Tp ret = *reinterpret_cast<const Tp*>(buff.data() + _curr);
	_curr += sizeof(Tp);

	return ret;
    }

    // *** instanciations of the function template fetch, because with g++-3.3
    // *** in it doesnt work in the GraphParser class

    inline unsigned char fetch_unsigned_char()
    { return fetch<unsigned char>(); }

    inline unsigned short fetch_unsigned_short()
    { return fetch<unsigned short>(); }

    inline unsigned int fetch_unsigned_int()
    { return fetch<unsigned int>(); }
       
    /// fetch a short string prefixed by an unsigned char.
    inline std::string fetchString()
    {
	unsigned char slen = fetch<unsigned char>();

	check_available(slen);
	std::string ret(buff.data() + _curr, slen);

	_curr += slen;
	return ret;
    }

    /// fetch a long string prefixed by an unsigned int.
    inline std::string fetchLongString()
    {
	unsigned int slen = fetch<unsigned int>();

	check_available(slen);
	std::string ret(buff.data() + _curr, slen);

	_curr += slen;
	return ret;
    }

    /// fetch a number of bytes from the buffer
    inline void fetchBytes(char *dest, unsigned int len)
    {
	check_available(len);
	memcpy(dest, buff.data() + _curr, len);

	_curr += len;
    }

    /// fetch a number of bytes from the buffer
    inline void fetchBytes(ByteBuffer &dest, unsigned int len)
    {
	check_available(len);
	dest.assign_copy(buff.data() + _curr, len);

	_curr += len;
    }

    /// fetch an AnyType value at the current position, the type to fetch must
    /// be set previously.
    void 	fetchAnyType(class AnyType &v);
};

} // namespace VGServer

#endif // VGS_ByteInBuffer_H
