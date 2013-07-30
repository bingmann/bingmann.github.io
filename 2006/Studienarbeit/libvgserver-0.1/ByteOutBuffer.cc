// $Id: ByteOutBuffer.cc 192 2006-06-15 08:00:53Z bingmann $

#include "ByteOutBuffer.h"
#include "AnyType.h"

#include <stdlib.h>
#include <string.h>

namespace VGServer {

void ByteOutBuffer::appendBytes(const void *indata, size_t inlen)
{
    if (buff.size() + inlen > buff.buffsize()) buff.grow(buff.size() + inlen);

    memcpy(buff.data() + buff.size(), indata, inlen);
    buff.set_size(buff.size() + inlen);
}

void ByteOutBuffer::appendAnyType(const class AnyType &v)
{
    switch(v.getType())
    {
    case ATTRTYPE_INVALID: assert(0); return;

    case ATTRTYPE_BOOL:
	// this is a special case: bit values are stored within the
	// default bitfield, this must be handeled differently.
	assert(0);
	return;

    case ATTRTYPE_CHAR:
	append<char>(v.getInteger());
	return;

    case ATTRTYPE_SHORT:
	append<short>(v.getInteger());
	return;

    case ATTRTYPE_INTEGER:
	append<int>(v.getInteger());
	return;

    case ATTRTYPE_LONG:
	append<long long>(v.getLong());
	return;

    case ATTRTYPE_BYTE:
	append<unsigned char>(v.getInteger());
	return;

    case ATTRTYPE_WORD:
	append<unsigned short>(v.getInteger());
	return;

    case ATTRTYPE_DWORD:
	append<unsigned int>(v.getInteger());
	return;

    case ATTRTYPE_QWORD:
	append<unsigned long long>(v.getUnsignedLong());
	return;

    case ATTRTYPE_FLOAT:
	append<float>(static_cast<float>(v.getDouble()));
	return;

    case ATTRTYPE_DOUBLE:
	append<double>(v.getDouble());
	return;

    case ATTRTYPE_STRING:
    {
	std::string sout = v.getString();
	append<unsigned char>(static_cast<unsigned char>(sout.size()));
	appendString(sout);
	return;
    }
    case ATTRTYPE_LONGSTRING:
    {
	std::string sout = v.getString();
	append<unsigned int>(static_cast<unsigned int>(sout.size()));
	appendString(sout);
	return;
    }
    }
    assert(0);
}

} // namespace VGServer
