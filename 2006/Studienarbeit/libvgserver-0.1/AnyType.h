// $Id: AnyType.h 165 2006-05-29 16:39:53Z bingmann $

#ifndef VGS_AnyType_H
#define VGS_AnyType_H

#include <string>
#include <stdexcept>
#include <functional>
#include <assert.h>

#include "GraphTypes.h"

namespace VGServer {

/** \ingroup Public
 * AnyType is the class which encapsules the different possible attribute
 * typed values. It tries to do automatic conversion between different-typed
 * input values. This could also be implemented using a polymorphic class
 * hierarchy */

class AnyType
{
private:
    attrtype_t		type;

    union
    {
	int		_int;
	unsigned int	_uint;
	long long	_long;
	unsigned long long _ulong;
	float		_float;
	double		_double;
	std::string*	_string;
    };
    
public:
    /// create a new AnyType object
    explicit inline AnyType(attrtype_t t)
    { 
	type = t;
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    _string = new std::string;
	}
    }

    // *** Constructors for the various types

    inline AnyType(bool b)
    {
	type = ATTRTYPE_BOOL;
	_int = b;
    }
     inline AnyType(char c)
    {
	type = ATTRTYPE_CHAR;
	_int = c;
    }
    inline AnyType(short s)
    {
	type = ATTRTYPE_SHORT;
	_int = s;
    }
    inline AnyType(int i)
    {
	type = ATTRTYPE_INTEGER;
	_int = i;
    }
    inline AnyType(long long l)
    {
	type = ATTRTYPE_LONG;
	_long = l;
    }
    inline AnyType(unsigned char c)
    {
	type = ATTRTYPE_BYTE;
	_uint = c;
    }
    inline AnyType(unsigned short s)
    {
	type = ATTRTYPE_WORD;
	_uint = s;
    }
    inline AnyType(unsigned int i)
    {
	type = ATTRTYPE_DWORD;
	_uint = i;
    }
    inline AnyType(unsigned long long l)
    {
	type = ATTRTYPE_QWORD;
	_ulong = l;
    }
    inline AnyType(float f)
    {
	type = ATTRTYPE_FLOAT;
	_float = f;
    }
    inline AnyType(double d)
    {
	type = ATTRTYPE_DOUBLE;
	_double = d;
    }
    inline AnyType(const char *s)
    {
	type = ATTRTYPE_STRING;
	_string = new std::string(s);
    }
    inline AnyType(const std::string &s)
    {
	type = ATTRTYPE_STRING;
	_string = new std::string(s);
    }

    /// destroy the object: free associated string memory
    inline ~AnyType()
    {
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    delete _string;
	    _string = NULL;
	}
    }
    
    /// copy constructor to deal with enclosed strings
    inline AnyType(const AnyType &a)
    {
	type = a.type;

	switch(type)
	{
	case ATTRTYPE_INVALID:
	    break;

	case ATTRTYPE_BOOL:
	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	    _int = a._int;
	    break;

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	    _uint = a._uint;
	    break;

	case ATTRTYPE_LONG:
	    _long = a._long;
	    break;

	case ATTRTYPE_QWORD:
	    _ulong = a._ulong;
	    break;

	case ATTRTYPE_FLOAT:
	    _float = a._float;
	    break;

	case ATTRTYPE_DOUBLE:
	    _double = a._double;
	    break;

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	    _string = new std::string(*a._string);
	    break;
	}
    }

    /// assignment operator to deal with enclosed strings
    inline AnyType& operator=(const AnyType &a)
    {
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    delete _string;
	    _string = NULL;
	}

	type = a.type;
	
	switch(type)
	{
	case ATTRTYPE_INVALID: assert(0); break;

	case ATTRTYPE_BOOL:
	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	    _int = a._int;
	    break;

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	    _uint = a._uint;
	    break;

	case ATTRTYPE_LONG:
	    _long = a._long;
	    break;

	case ATTRTYPE_QWORD:
	    _ulong = a._ulong;
	    break;

	case ATTRTYPE_FLOAT:
	    _float = a._float;
	    break;

	case ATTRTYPE_DOUBLE:
	    _double = a._double;
	    break;

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	    _string = new std::string(*a._string);
	    break;
	}
	return *this;
    }

    /// comparison operator: compares values _and_ types
    bool operator==(const AnyType &a) const;

    /// comparison operator: compares values _and_ types
    inline bool operator!=(const AnyType &a) const
    { return not(*this == a); }

    /// return the type of the object
    inline attrtype_t	getType() const
    { return type; }

    /// changes the current type: returns false if the current value could not
    /// be converted to the new type, the new type is set nonetheless.
    bool	convertType(attrtype_t t);

    /// changes the current type and resets the contents without attempting to
    /// convert the enclosed value.
    void	resetType(attrtype_t t);

    // *** attrtype_t to string and string to attrtype_t functions

    /// returns true if the attrtype_t is a valid type
    static bool isValidAttrtype(attrtype_t at);
    
    /// returns the attrtype enum value of a string, throws ConversionException
    /// if it cannot be determined.
    static attrtype_t stringToType(const std::string &s)
    { return stringToType(s.c_str()); }

    /// returns the attrtype enum value of a string, throws ConversionException
    /// if it cannot be determined.
    static attrtype_t stringToType(const char *s);

    /// returns a string for each attrtype_t.
    static std::string getTypeString(attrtype_t at);

    /// returns a string for each this AnyType's type.
    std::string getTypeString() const
    { return getTypeString(type); }

    // *** Type and Value Length Functions
    
    /// return the storage length of the type in bytes
    inline int	getTypeLength() const
    { return getTypeLength(type); }

    /// return the storage length of the type in bytes
    static int	getTypeLength(attrtype_t t);

    /// boolean check if this type is of fixed-length
    static bool isFixedLength(attrtype_t t)
    { return getTypeLength(t) >= 0; }

    /// boolean check if this type is of fixed-length
    inline bool	isFixedLength() const
    { return isFixedLength(type); }

    /// return the storage length of this value (mark the different to getTypeLength()
    unsigned int getValueLength() const;

    /// sets the value, converting the input if necessary. returns false if the
    /// input could not be converted into the right type.
    bool	setInteger(int i);

    bool	setLong(long long l);

    bool	setDouble(double d);

    bool	setString(const std::string &s);
    
    bool	setStringQuoted(const std::string &s);

    /// return the enclosed value in different types, converting if necessary
    /// if the enclosed value cannot be converted these functions throw a
    /// ConversionError exception.
    bool	getBoolean() const;

    int		getInteger() const;

    unsigned int getUnsignedInteger() const;

    long long 	getLong() const;

    unsigned long long getUnsignedLong() const;

    double	getDouble() const;

    std::string	getString() const;
    
    // *** Unary Operators
    
    /// unary prefix - operator. Converts the AnyType to a numeric value.
    AnyType 	operator-() const;

    // *** Binary Operators, these will convert the two operands to the largest
    // *** common type of the same field.

    template <template <typename Type> class Operator, char OpName>
    AnyType	binary_arith_op(const AnyType &b) const;
    
    inline AnyType	operator+(const AnyType &b) const
    { return binary_arith_op<std::plus, '+'>(b); }

    inline AnyType	operator-(const AnyType &b) const
    { return binary_arith_op<std::minus, '-'>(b); }

    inline AnyType	operator*(const AnyType &b) const
    { return binary_arith_op<std::multiplies, '*'>(b); }

    inline AnyType	operator/(const AnyType &b) const
    { return binary_arith_op<std::divides, '/'>(b); }

    template <template <typename Type> class Operator, int OpNum>
    bool	binary_comp_op(const AnyType &b) const;

    // dont use the operators themselves, because == is defined differently above.
    inline bool		op_equal_to(const AnyType &b) const
    { return binary_comp_op<std::equal_to, 0>(b); }

    inline bool		op_not_equal_to(const AnyType &b) const
    { return binary_comp_op<std::not_equal_to, 1>(b); }

    inline bool		op_less(const AnyType &b) const
    { return binary_comp_op<std::less, 2>(b); }

    inline bool		op_greater(const AnyType &b) const
    { return binary_comp_op<std::greater, 3>(b); }

    inline bool		op_less_equal(const AnyType &b) const
    { return binary_comp_op<std::less_equal, 4>(b); }

    inline bool		op_greater_equal(const AnyType &b) const
    { return binary_comp_op<std::greater_equal, 5>(b); }

};

/** ConversionException is an exception class thrown by some combinations of
 * get and set in AnyType. \ingroup Exception */

class ConversionException : public GraphException
{
public:
    inline ConversionException(const std::string &s) throw()
	: GraphException(s)
    { }
};

} // namespace VGServer

#endif // VGS_AnyType_H
