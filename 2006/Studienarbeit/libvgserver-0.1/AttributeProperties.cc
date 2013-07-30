// $Id: AttributeProperties.cc 241 2006-07-05 07:29:52Z bingmann $

#include "AttributeProperties.h"
#include "GraphProperties.h"
#include "AttributeBlob.h"
#include "AttributeBlob_impl.h"
#include "ByteOutBuffer.h"

#include <stdlib.h>
#include <string.h>

namespace VGServer {

unsigned int AttributePropertiesList::calcDefaultBytes(unsigned int n)
{
    if (n == 0) return 0;
    return ((n-1) / 8) + 1;
}

void AttributePropertiesList::calcAttributeLookups()
{
    // clears attrname map
    attrname_map.clear();

    std::vector<class AttributeProperties>::iterator attr = begin();

    // run from front to back of the attributes and
    // 1) fill in the accelerated lookup index for fixed, no-default attributes
    // 2) after the first with-default attribute start counting default-bits

    int lookup = 0;
    
    unsigned int defbitnum = 0;

    unsigned int attrid = 0;

    while(attr != end())
    {
	if (attr->getType() == ATTRTYPE_BOOL) {
	    // bool type gets a default bit num to store it's value in. it does
	    // not interfere with direct lookups
	    attr->lookup = -1;
	    attr->varying = false;
	}
	else if (lookup >= 0 and (attr->varying and attr->isFixedLength()))
	{
	    // fill in the current lookup index and advance it.
	    attr->lookup = lookup;
	    lookup += attr->getTypeLength();
	}
	else {
	    // one of the tests failed -> no more accelerated lookups
	    attr->lookup = lookup = -1;
	}

	if (not attr->varying)
	{
	    attr->defbitnum = defbitnum++;
	}

	// save name in map.
	attrname_map[attr->name] = attrid++;
	
	attr++;
    }

    // in the special case that we have zero attributes, then all vertx/edge
    // objects would be considered deleted. therefore we'll add just one empty
    // byte instead of a "deleted" flag.
    if (lookup == 0 and defbitnum == 0)
	defbitnum++;

    // now that we know how many default bytes we need, advance the direct
    // lookup indices
    if (defbitnum > 0)
    {
	unsigned int defbytenum = calcDefaultBytes(defbitnum);

	for(attr = begin(); attr != end() and attr->lookup >= 0; attr++)
	{
	    attr->lookup += defbytenum;
	}
    }

    this->defbits = defbitnum;
    this->defbytes = calcDefaultBytes(defbitnum);
}

int AttributePropertiesList::lookupAttributeName(const std::string &s) const
{
    std::map<std::string, unsigned int>::const_iterator it = attrname_map.find(s);

    if (it == attrname_map.end()) return -1;
    else return it->second;
}

// two functions which write the binary representation of the
// AttributePropertiesList. See text docus for a longer explanation.

void AttributeProperties::appendBinaryFormat(class ByteOutBuffer &bob) const
{
/* Attribute Binary Representation:
   
+--------+---------+---------------+----------------+
| type   | namelen | name          | default value  |
+--------+---------+---------------+----------------+
| 1 byte | 1 byte  | namelen bytes | in attr repres |

type is the value from the attrtype enum.

*/
    bob.append<unsigned char>(getType());

    bob.append<unsigned char>(static_cast<unsigned char>(name.size()));
    bob.appendString(name);

    // put default value.
    if (getType() == ATTRTYPE_BOOL)
    {
	bob.append<unsigned char>(getInteger());
    }
    else {
	bob.appendAnyType(*this);
    }
}

void AttributePropertiesList::appendBinaryFormat(class ByteOutBuffer &bob) const
{
/* Attribute Binary Representation:
   
+---------+----------+--------------------+
| attrlen | defbytes | attribute repres.  |
+---------+----------+--------------------+
| 2 bytes | 1 byte   | variable           |

the number of default bytes are always included.

*/
    bob.append<unsigned short>(static_cast<unsigned short>(size()));
    bob.append<unsigned char>(defbytes);

    for(const_iterator ai = begin(); ai != end(); ai++) {
	ai->appendBinaryFormat(bob);
    }
}

void GraphProperties::appendBinaryFormat(class ByteOutBuffer &bob) const
{
/* Graph Properties Binary Representation:
   
+------+----------+------+--------------------+------+------------------+
| 0x10 | directed | 0x01 | vertex attr repres | 0x02 | edge attr repres |
+------+----------+------+--------------------+------+------------------+
| 1b   | 1 byte   | 1b   | variable           | 1b   | variable         |

*/
    bob.append<unsigned char>(0x10);
    bob.append<unsigned char>(directed);

    bob.append<unsigned char>(0x01);
    vertexattr.appendBinaryFormat(bob);

    bob.append<unsigned char>(0x02);
    edgeattr.appendBinaryFormat(bob);

    bob.append<unsigned char>(0x00);
}

template <typename AttributeTinyBlob>
AttributeTinyBlob AttributePropertiesList::createBlankAttributeBlob() const
{
    AttributeTinyBlob ab;
    unsigned int len = 0;

    // put the default bit field at the beginning
    ab.zero(len, defbytes);
    len += defbytes;

    // put in the values of the varying attributes
    unsigned int ai;
    for (ai = 0; ai < size(); ai++)
    {
	if (not (*this)[ai].varying) break;
	
	ab.putAnyType(len, (*this)[ai]);
	len += (*this)[ai].getValueLength();
    }

    // continue for the rest by setting their default bits.
    for (; ai < size(); ai++)
    {
	assert( not (*this)[ai].varying );
	assert( (*this)[ai].defbitnum < defbits );

	if ((*this)[ai].getType() == ATTRTYPE_BOOL)
	{
	    // set the boolean in the default bitfield to the attribute's
	    // default boolean value.
	    ab.putBool(0, (*this)[ai].defbitnum, (*this)[ai].getInteger() != 0);
	}
	else
	{
	    ab.putBool(0, (*this)[ai].defbitnum, true);
	}
    }

    return ab;
}

// two instanciations for the two TinyBlob types: (this creates the above's function code in the .o file)

template AttributeVertexTinyBlob AttributePropertiesList::createBlankAttributeBlob<AttributeVertexTinyBlob>() const;

template AttributeEdgeTinyBlob AttributePropertiesList::createBlankAttributeBlob<AttributeEdgeTinyBlob>() const;

} // namespace VGServer
