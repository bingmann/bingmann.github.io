// $Id: ByteOutBuffer.h 127 2006-05-13 15:30:07Z bingmann $

#ifndef VGS_ByteOutBuffer_H
#define VGS_ByteOutBuffer_H

#include <stdlib.h>
#include <assert.h>

#include <utility>

#include "ByteBuffer.h"

namespace VGServer {

/** ByteOutBuffer is a cursor class to ByteBuffer implementing growing byte
 * oriented memory buffer interface used in the graph server when constructing
 * the client output. */

class ByteOutBuffer
{
protected:
    /// reference to manipulated ByteBuffer
    ByteBuffer	&buff;

public:
    /// create a new cursor object
    inline ByteOutBuffer(ByteBuffer &_buff)
	: buff(_buff)
    {
    }

    /// copy constructor: create a new duplicate object.
    inline ByteOutBuffer(const ByteOutBuffer &o)
	: buff(o.buff)
    {
    }

    /// append a memory range to the buffer
    void appendBytes(const void *data, size_t len);

    /// append the contents of a different object to this one (overload the
    /// template function)
    inline void appendByteBuffer(const ByteBuffer &bb)
    { appendBytes(bb.data(), bb.size()); }

    /// append to contents of a std::string !! excluding the null !! (overload
    /// ing the template function)
    inline void appendString(const std::string &s)
    { appendBytes(s.data(), s.size()); }

    /// append a single item of the template type T to the buffer. Be careful
    /// with implicit type conversions!
    template <class T>
    inline void append(T item)
    {
	if (buff.size() + sizeof(T) > buff.buffsize()) buff.grow(buff.size() + sizeof(T));

	*reinterpret_cast<T*>(buff.data() + buff.size()) = item;
	buff.set_size(buff.size() + sizeof(T));
    }

    // *** Functions with fixed append size

    /// append a single byte (8 bits) to the buffer
    inline void append_8(char c)
    { append<char>(c); }

    /// append two bytes = one word (16 bits) to the buffer
    inline void append_16(short s)
    { append<short>(s); }

    /// append four bytes = one dword (32 bits) to the buffer
    inline void append_32(int i)
    { append<int>(i); }

    /// append four bytes = one dword (32 bits) to the buffer
    inline void append_64(long long l)
    { append<long long>(l); }

    // *** Functions to append AnyTypes
    
    /// appends a defined AnyType at the position i
    void appendAnyType(const class AnyType &v);
};



} // namespace VGServer

#endif // VGS_ByteOutBuffer_H
