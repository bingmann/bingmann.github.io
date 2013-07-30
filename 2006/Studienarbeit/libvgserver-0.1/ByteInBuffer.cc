// $Id: ByteInBuffer.cc 179 2006-06-07 06:04:17Z bingmann $

#include "ByteInBuffer.h"
#include "AnyType.h"

#include <stdlib.h>
#include <string.h>

namespace VGServer {

void ByteInBuffer::fetchAnyType(class AnyType &v)
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
	v.setInteger(fetch<char>());
	return;

    case ATTRTYPE_SHORT:
	v.setInteger(fetch<short>());
	return;

    case ATTRTYPE_INTEGER:
	v.setInteger(fetch<int>());
	return;

    case ATTRTYPE_LONG:
	v.setLong(fetch<long long>());
	return;

    case ATTRTYPE_BYTE:
	v.setInteger(fetch<unsigned char>());
	return;

    case ATTRTYPE_WORD:
	v.setInteger(fetch<unsigned short>());
	return;

    case ATTRTYPE_DWORD:
	v.setInteger(fetch<unsigned int>());
	return;

    case ATTRTYPE_QWORD:
	v.setLong(fetch<unsigned long long>());
	return;

    case ATTRTYPE_FLOAT:
	v.setDouble(fetch<float>());
	return;

    case ATTRTYPE_DOUBLE:
	v.setDouble(fetch<double>());
	return;

    case ATTRTYPE_STRING:
	v.setString(fetchString());
	return;

    case ATTRTYPE_LONGSTRING:
	v.setString(fetchLongString());
	return;
    }
    assert(0);
}

} // namespace VGServer
