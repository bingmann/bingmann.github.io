// $Id: AttributeBlob_impl.h 165 2006-05-29 16:39:53Z bingmann $

#ifndef VGS_AttributeBlob_Impl_H
#define VGS_AttributeBlob_Impl_H

#include "AttributeBlob.h"

namespace VGServer {

/** \file 
 * This file contains implementations of the larger function of
 * AttributeBlob. It should be included in source files which actually used
 * these large function. */

/// reads a defined AnyType at the position i
template <typename AllocPolicy>
inline void TpAttributeBlob<AllocPolicy>::getAnyType(unsigned int i, AnyType &v) const
{
    switch(v.getType())
    {
    case ATTRTYPE_INVALID: assert(0); return;

    case ATTRTYPE_BOOL:
	// this is a special case: bit values are stored within the default
	// bitfield, so the index i alone is insufficient to fetch the boolean
	// value. getBool must be used directly.
	assert(0);
	return;

    case ATTRTYPE_CHAR:
	v.setInteger(get<char>(i));
	return;

    case ATTRTYPE_SHORT:
	v.setInteger(get<short>(i));
	return;

    case ATTRTYPE_INTEGER:
	v.setInteger(get<int>(i));
	return;

    case ATTRTYPE_LONG:
	v.setLong(get<long long>(i));
	return;

    case ATTRTYPE_BYTE:
	v.setInteger(get<unsigned char>(i));
	return;

    case ATTRTYPE_WORD:
	v.setInteger(get<unsigned short>(i));
	return;

    case ATTRTYPE_DWORD:
	v.setInteger(get<unsigned int>(i));
	return;

    case ATTRTYPE_QWORD:
	v.setLong(get<unsigned long long>(i));
	return;

    case ATTRTYPE_FLOAT:
	v.setDouble(get<float>(i));
	return;

    case ATTRTYPE_DOUBLE:
	v.setDouble(get<double>(i));
	return;

    case ATTRTYPE_STRING:
	v.setString(getString(i));
	return;

    case ATTRTYPE_LONGSTRING:
	v.setString(getLongstring(i));
	return;

    }
    assert(0);
}

/// saves a defined AnyType at the position i
template <typename AllocPolicy>
inline void TpAttributeBlob<AllocPolicy>::putAnyType(unsigned int i, const AnyType &v)
{
    switch(v.getType())
    {
    case ATTRTYPE_INVALID: assert(0); return;

    case ATTRTYPE_BOOL:
	// this is a special case: bit values are stored within the
	// default. putBool must be used directly.
	assert(0);
	return;

    case ATTRTYPE_CHAR:
	put<char>(i, v.getInteger());
	return;

    case ATTRTYPE_SHORT:
	put<short>(i, v.getInteger());
	return;

    case ATTRTYPE_INTEGER:
	put<int>(i, v.getInteger());
	return;

    case ATTRTYPE_LONG:
	put<long long>(i, v.getLong());
	return;

    case ATTRTYPE_BYTE:
	put<unsigned char>(i, v.getUnsignedInteger());
	return;

    case ATTRTYPE_WORD:
	put<unsigned short>(i, v.getUnsignedInteger());
	return;

    case ATTRTYPE_DWORD:
	put<unsigned int>(i, v.getUnsignedInteger());
	return;

    case ATTRTYPE_QWORD:
	put<unsigned long long>(i, v.getUnsignedLong());
	return;

    case ATTRTYPE_FLOAT:
	put<float>(i, static_cast<float>(v.getDouble()));
	return;

    case ATTRTYPE_DOUBLE:
	put<double>(i, v.getDouble());
	return;

    case ATTRTYPE_STRING:
	putString(i, v.getString());
	return;

    case ATTRTYPE_LONGSTRING:
	putLongstring(i, v.getString());
	return;

    }
    assert(0);
}

/// Calculates the length of a valid attribute value chain at position i
template <typename AllocPolicy>
inline unsigned int TpAttributeBlob<AllocPolicy>::getAttrChainLength(unsigned int i, const AttributePropertiesList &attrlist) const
{
    unsigned int p = i + attrlist.defbytes;

    for(unsigned int ai = 0; ai < attrlist.size(); ai++)
    {
	if (attrlist[ai].varying)
	{
	    assert(attrlist[ai].isFixedLength());
	    p += attrlist[ai].getTypeLength();
	}
	else
	{
	    assert(attrlist.defbits > attrlist[ai].defbitnum);
	    if (getBool(i, attrlist[ai].defbitnum))
	    {
		// if the default bit is set, then no space is used in the
		// attribute array
	    }
	    else
	    {
		switch(attrlist[ai].getType())
		{
		default: assert(0); break;

		CASE_FIXED_LENGTH:
		    // easy case
		    p += attrlist[ai].getTypeLength();
		    break;

		    // if this is a string or long string: we have to fetch the
		    // string's length from the attribute blob.
		case ATTRTYPE_STRING:
		    p += get<unsigned char>(p);
		    break;

		case ATTRTYPE_LONGSTRING:
		    p += get<unsigned int>(p);
		    break;
		}
	    }
	}
    }

    return p - i;
}

/// retrieves the attribute value attrid from an attribute chain at position i
template <typename AllocPolicy>
inline AnyType TpAttributeBlob<AllocPolicy>::getAttrChainValue(unsigned int chidx, unsigned int attrid, const AttributePropertiesList &attrlist) const
{
    if (attrlist.size() < attrid) throw AttributeIdException();

    // alias for this attribute's properties
    const AttributeProperties &ap = attrlist[attrid];

    // create correctly typed return object
    class AnyType value(ap.getType());

    if (ap.lookup >= 0)	// use the direct lookup index
    {
	// retrieve the typed value
	getAnyType(chidx + ap.lookup, value);
	return value;
    }

    // if it is a boolean type, the value is stored in the default bitfield
    if (ap.getType() == ATTRTYPE_BOOL)
    {
	value.setInteger( getBool(chidx, ap.defbitnum) );
	return value;
    }

    // check if the default bit is cleared: then return the default attribute
    // value.
    if (getBool(chidx, ap.defbitnum))
    {
	value = ap;
	return value;
    }

    // otherwise we have to iterate over the attribute values to find the
    // correct offset.
    unsigned int idx = chidx + attrlist.defbytes;

    for(unsigned int _i = 0; _i < attrid; _i++)
    {
	const AttributeProperties &aiter = attrlist[_i];

	// attributes marked as varying are always in the blob
	if (aiter.varying)
	{
	    assert(aiter.isFixedLength());
	    idx += aiter.getTypeLength();
	}
	else
	{
	    if (getBool(chidx, aiter.defbitnum))
	    {
		// if the default bit is set, then no space is used in the
		// attribute array
	    }
	    else
	    {
		switch(aiter.getType())
		{
		default: assert(0); break;

		CASE_FIXED_LENGTH:
		    // easy case
		    idx += aiter.getTypeLength();
		    break;
			
		// this is a string or long string: we have to fetch the
		// string's length from the attribute blob.
		case ATTRTYPE_STRING:
		    idx += get<unsigned char>(idx);
		    break;

		case ATTRTYPE_LONGSTRING:
		    idx += get<unsigned int>(idx);
		    break;
		}

	    }
	}
    }

    // well if everything worked right, then idx is now the offset
    getAnyType(idx, value);
    return value;
}

template <typename AllocPolicy>
inline bool TpAttributeBlob<AllocPolicy>::putAttrChainValue(const AttributePropertiesList &attrlist,
							    unsigned int attrid, const AnyType &value)
{
    if (attrid >= attrlist.size()) throw AttributeIdException();

    if (value.getType() != attrlist[attrid].getType())
    {
	// convert input value to attribute type, this could throw a
	// ConversionException.
	AnyType conv = value;
	conv.convertType(attrlist[attrid].getType());
	
	return putAttrChainValue(attrlist, attrid, conv);
    }

    // alias for the attribute properties
    const AttributeProperties &attrprop = attrlist[attrid];

    // easy case: the value is a boolean, so it is in the default bitfield.
    if (value.getType() == ATTRTYPE_BOOL)
    {
	putBool(0, attrprop.defbitnum, value.getInteger() != 0);
	return true;
    }
    
    // another fast and easy case: direct lookup attributes
    if (attrprop.lookup >= 0)
    {
	assert(attrprop.isFixedLength());

	// overwrite value at a direct lookup
	putAnyType(attrprop.lookup, value);
	return true;
    }

    // stupid case in which the default bit is set and the new value is also
    // the default value. Thus this is a noop.
    if (getBool(0, attrprop.defbitnum) and value == attrprop)
	return true;

    // else we have to run through the default bitfield to find the
    // right place.
    unsigned int p = attrlist.defbytes;

    for(unsigned int ai = 0; ai < attrid; ai++)
    {
	if (attrlist[ai].varying)
	{
	    assert(attrlist[ai].isFixedLength());
	    p += attrlist[ai].getTypeLength();
	}
	else
	{
	    if (getBool(0, attrlist[ai].defbitnum))
	    {
		// if the default bit is set, then no space is used in the
		// attribute array
	    }
	    else
	    {
		switch(attrlist[ai].getType())
		{
		default: assert(0); break;

		CASE_FIXED_LENGTH:
		    // easy case
		    p += attrlist[ai].getTypeLength();
		    break;

		// if this is a string or long string: we have to fetch the
		// string's length from the attribute blob.
		case ATTRTYPE_STRING:
		    p += get<unsigned char>(p);
		    break;

		case ATTRTYPE_LONGSTRING:
		    p += get<unsigned int>(p);
		    break;
		}
	    }
	}
    }

    // p should now be the index of the attribute value, if it's default bit is
    // cleared.

    assert(not attrprop.varying);

    if (getBool(0, attrprop.defbitnum))
    {
	if (value == attrprop)
	{
	    // no need for action: setting default value which is already set
	    assert(0); // assert because this should have been handled above.
	}
	else
	{
	    // default bit of our attribute is set, so we have to make room in the
	    // blob for it's new value.
	
	    putBool(0, attrprop.defbitnum, false);

	    // same procedure for fixed sized and variable sized attributes, the
	    // work is done by AnyType::getValueLength.

	    move(p, p + value.getValueLength(), size() - p);

	    putAnyType(p, value);
	}
    }
    else
    {
	// the default bit was cleared, so the attribute already has a
	// non-default value.

	if (value != attrprop)
	{
	    // the new value is not the default value.

	    switch(value.getType())
	    {
	    default: assert(0); break;

	    CASE_FIXED_LENGTH:
	    {
		// simply overwrite fixed size values
		putAnyType(p, value);
		break;
	    }

	    // for variable sized attributes, we must move the following
	    // attribute values.

	    case ATTRTYPE_STRING:
	    {
		unsigned char oldlen = sizeof(unsigned char) + get<unsigned char>(p);

		if (oldlen < value.getValueLength() or oldlen > value.getValueLength())
		{
		    // make more room by moving trailing attribute values.
		    move(p + oldlen, p + value.getValueLength(),
			 size() - (p + oldlen));
		}

		// now the space fits exactly
		putAnyType(p, value);
		break;
	    }

	    case ATTRTYPE_LONGSTRING:
	    {
		unsigned int oldlen = sizeof(unsigned int) + get<unsigned int>(p);

		if (oldlen < value.getValueLength() or oldlen > value.getValueLength())
		{
		    // make more room by moving trailing attribute values.
		    move(p + oldlen, p + value.getValueLength(),
			 size() - (p + oldlen));
		}

		// now the space fits exactly
		putAnyType(p, value);
		break;
	    }
	    }
	}
	else
	{
	    // new value is the default value, so we set the default bit and
	    // erase any used memory.
	    
	    putBool(0, attrprop.defbitnum, true);
	    
	    // calculate the length to splice from the attribute value blob
	    unsigned int len = 0;

	    switch(value.getType())
	    {
	    default: assert(0); break;

	    CASE_FIXED_LENGTH:
		len = attrprop.getTypeLength();
		break;

	    // for variable sized attributes, we must check the length value in
	    // the array
	    case ATTRTYPE_STRING:
		len = get<unsigned char>(p);
		break;

	    case ATTRTYPE_LONGSTRING:
		len = get<unsigned int>(p);
		break;
	    }

	    move(p + len, p, size() - (p + len));
	}
    }
    return true;
}

} // namespace VGServer

#endif // VGS_AttributeBlob_Impl_H


