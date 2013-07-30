// $Id: AttributeProperties.h 85 2006-04-02 20:28:44Z bingmann $

#ifndef VGS_AttributeProperties_H
#define VGS_AttributeProperties_H

#include <string>
#include <vector>
#include <map>

#include "GraphTypes.h"
#include "AnyType.h"
#include "AttributeBlob_fwd.h"

namespace VGServer {

/** AttributeProperties specifies the name, type and possibly more properties
 * of an attribute. Both vertex and edge attributes are specified by the same
 * class, as they are stored and selected using the same methods.
 * \ingroup Public */

class AttributeProperties : public AnyType
{
public:
    std::string		name;
    
    /// marks this attribute as having no default value. it does not have a bit
    /// in the default-bitfield.
    bool		varying;

    /// this is the zero-based index of the default bit assigned to this
    /// attribute.
    unsigned int	defbitnum;

    // if this is a no-default, fixed length attribute and every other
    // attribute before it is no-default and fixed too, then this is the direct
    // lookup index within the attribute chain including the
    // default-bitfield. Else it is -1.
    int			lookup;

public:
    /// empty default constructor, sets the type to INVALID
    AttributeProperties()
	: AnyType(ATTRTYPE_INVALID)
    { }

    /// constructs an attribute with the given name and attribute type, but
    /// doesnt set a value. you have to set a value using AnyType's
    /// set... functions.
    explicit AttributeProperties(const std::string& attrname, attrtype_t attrtype, bool _varying=false)
	: AnyType(attrtype), name(attrname), varying(_varying)
    {
    }

    /// constructs an attribute with the given AnyType (type,value) tupel as
    /// default value.
    explicit AttributeProperties(const std::string& attrname, const AnyType &def, bool _varying=false)
	: AnyType(def), name(attrname), varying(_varying)
    {
    }

    /// overloaded resetType function from AnyType to reset the
    /// properties. excluding the name!. this is needed for the parser
    void	resetType(attrtype_t t)
    {
	AnyType::resetType(t);
	varying = false;
	defbitnum = 0;
	lookup = 0;
    }

    /// concatenate the binary representation of this attribute to the buffer
    void	appendBinaryFormat(class ByteOutBuffer &bob) const;
};

/** AttributePropertiesList is a container class derived from std::vector
 * enclosing the list of attributes. It extends the functionality by
 * pre-calculating the necessary offsets into the AttributeBlob and has special
 * values like how many default bytes the attribute list needs.
 * \ingroup Public  */

class AttributePropertiesList : public std::vector<class AttributeProperties>
{
public:
    /// the number of default bits used in the attribute list.
    unsigned int				defbits;

    /// number of default bytes used in the attribute list.
    unsigned int				defbytes;

private:
    /// maps attribute names case sensitive to attrids.
    std::map<std::string, unsigned int>		attrname_map;

public:    
    /// override vector::clear()
    void clear() {
	std::vector<class AttributeProperties>::clear();
	attrname_map.clear();
	defbits = defbytes = 0;
    }

    /// this does the actual pre-calculation of the attribute offsets, and
    /// saves the attribute names in the attrname_map;
    void		calcAttributeLookups();

    /// looks the attribute id of an attribute name up, returns -1 if it could
    /// not be found.
    int			lookupAttributeName(const std::string &s) const;

    /// calculate default bytes needed to store n default bits
    static unsigned int calcDefaultBytes(unsigned int n);

    /// create a blank AttributeBlob containing the default bitfield and
    /// initialized varying attributes.
    template <typename AttributeBlob>
    AttributeBlob createBlankAttributeBlob() const;

    /// concatenate the binary representation of this attribute list to the
    /// buffer
    void	appendBinaryFormat(class ByteOutBuffer &bob) const;
};

/** AttributeIdException is thrown by setVertexAttr and setEdgeAttr functions
 * when supplied with an invalid attribute id. \ingroup Exception */

class AttributeIdException : public std::exception
{
};

} // namespace VGServer

#endif // VGS_AttributeProperties_H
