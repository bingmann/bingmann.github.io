// $Id: AnyType.cc 172 2006-06-04 18:03:21Z bingmann $

#include "AnyType.h"
#include "GraphPort.h"

#include <stdlib.h>
#include <functional>

namespace VGServer {

/// static function instead of using the non-standard utoa
static char *convert_unsigned_decimal(unsigned int val, char *buf, int bufsize)
{
    int n = bufsize-1;
    buf[n] = 0;
    n--;
	
    while(n > 0)
    {
	buf[n] = "0123456789"[val % 10];
	val /= 10;
	if (val == 0) break;
	n--;
    }

    return buf+n;
}

/// static function instead of using the non-standard itoa
static char *convert_signed_decimal(int val, char *buf, int bufsize)
{
    int n = bufsize-1;
    buf[n] = 0;
    n--;

    char sign = 0;
    if (val < 0) {
	sign = '-';
	val = -val;
    }

    while(n > 0)
    {
	buf[n] = "0123456789"[val % 10];
	val /= 10;
	if (val == 0) break;
	n--;
    }

    if (sign) buf[--n] = sign;

    return buf+n;
}

/// static function instead of using the non-standard utoa
static char *convert_unsigned64_decimal(unsigned long long val, char *buf, int bufsize)
{
    int n = bufsize-1;
    buf[n] = 0;
    n--;
	
    while(n > 0)
    {
	buf[n] = "0123456789"[val % 10];
	val /= 10;
	if (val == 0) break;
	n--;
    }

    return buf+n;
}

/// static function instead of using the non-standard itoa
static char *convert_signed64_decimal(long long val, char *buf, int bufsize)
{
    int n = bufsize-1;
    buf[n] = 0;
    n--;

    char sign = 0;
    if (val < 0) {
	sign = '-';
	val = -val;
    }

    while(n > 0)
    {
	buf[n] = "0123456789"[val % 10];
	val /= 10;
	if (val == 0) break;
	n--;
    }

    if (sign) buf[--n] = sign;

    return buf+n;
}

bool AnyType::operator==(const AnyType &a) const
{
    if (type != a.type) return false;

    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return (_int == a._int);

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return (_uint == a._uint);

    case ATTRTYPE_LONG:
	return (_long == a._long);

    case ATTRTYPE_QWORD:
	return (_ulong == a._ulong);

    case ATTRTYPE_FLOAT:
	return (_float == a._float);

    case ATTRTYPE_DOUBLE:
	return (_double == a._double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
	assert(_string && a._string);
	return (*_string == *a._string);
    }
    assert(0);
    return false;
}

attrtype_t AnyType::stringToType(const char* s)
{
    // stupid method, but maybe faster than using a biggy std::map with a
    // case-insensitive equality.

    if (g_strcasecmp(s, "bool") == 0)
	return ATTRTYPE_BOOL;

    if (g_strcasecmp(s, "char") == 0)
	return ATTRTYPE_CHAR;
    if (g_strcasecmp(s, "short") == 0)
	return ATTRTYPE_SHORT;
    if (g_strcasecmp(s, "integer") == 0)
	return ATTRTYPE_INTEGER;
    if (g_strcasecmp(s, "long") == 0)
	return ATTRTYPE_LONG;

    if (g_strcasecmp(s, "byte") == 0)
	return ATTRTYPE_BYTE;
    if (g_strcasecmp(s, "word") == 0)
	return ATTRTYPE_WORD;
    if (g_strcasecmp(s, "dword") == 0)
	return ATTRTYPE_DWORD;
    if (g_strcasecmp(s, "qword") == 0)
	return ATTRTYPE_QWORD;
    
    if (g_strcasecmp(s, "float") == 0)
	return ATTRTYPE_FLOAT;
    if (g_strcasecmp(s, "double") == 0)
	return ATTRTYPE_DOUBLE;

    if (g_strcasecmp(s, "string") == 0)
	return ATTRTYPE_STRING;
    if (g_strcasecmp(s, "longstring") == 0)
	return ATTRTYPE_LONGSTRING;

    throw(ConversionException("Invalid type name."));
}

bool AnyType::isValidAttrtype(attrtype_t at)
{
    switch(at)
    {
    default:
    case ATTRTYPE_INVALID:
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    case ATTRTYPE_LONG:
    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    case ATTRTYPE_QWORD:
    case ATTRTYPE_FLOAT:
    case ATTRTYPE_DOUBLE:
    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
	return true;
    }
}

std::string AnyType::getTypeString(attrtype_t at)
{
    switch(at)
    {
    case ATTRTYPE_INVALID:	return "invalid";
    case ATTRTYPE_BOOL:		return "bool";
    case ATTRTYPE_CHAR:		return "char";
    case ATTRTYPE_SHORT:	return "short";
    case ATTRTYPE_INTEGER:	return "integer";
    case ATTRTYPE_LONG:		return "long";
    case ATTRTYPE_BYTE:		return "byte";
    case ATTRTYPE_WORD:		return "word";
    case ATTRTYPE_DWORD:	return "dword";
    case ATTRTYPE_QWORD:	return "qword";
    case ATTRTYPE_FLOAT:	return "float";
    case ATTRTYPE_DOUBLE:	return "double";
    case ATTRTYPE_STRING:	return "string";
    case ATTRTYPE_LONGSTRING:	return "longstring";
    }
    return "unknown";
}

int AnyType::getTypeLength(attrtype_t t)
{
    switch(t)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return 0;

    case ATTRTYPE_BOOL:
	// weird value. beware!
	return 0;

    case ATTRTYPE_CHAR:
	return sizeof(char);

    case ATTRTYPE_BYTE:
	return sizeof(unsigned char);

    case ATTRTYPE_SHORT:
	return sizeof(short);

    case ATTRTYPE_WORD:
	return sizeof(unsigned short);

    case ATTRTYPE_INTEGER:
	return sizeof(int);

    case ATTRTYPE_DWORD:
	return sizeof(unsigned int);

    case ATTRTYPE_LONG:
	return sizeof(long long);

    case ATTRTYPE_QWORD:
	return sizeof(unsigned long long);

    case ATTRTYPE_FLOAT:
	return sizeof(float);

    case ATTRTYPE_DOUBLE:
	return sizeof(double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
	return -1;

    }
    assert(0);
    return -1;
}

unsigned int AnyType::getValueLength() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return 0;

    case ATTRTYPE_BOOL:
	// weird value. beware!
	return 0;

    case ATTRTYPE_CHAR:
	return sizeof(char);

    case ATTRTYPE_BYTE:
	return sizeof(unsigned char);

    case ATTRTYPE_SHORT:
	return sizeof(short);

    case ATTRTYPE_WORD:
	return sizeof(unsigned short);

    case ATTRTYPE_INTEGER:
	return sizeof(int);

    case ATTRTYPE_DWORD:
	return sizeof(unsigned int);

    case ATTRTYPE_LONG:
	return sizeof(long long);

    case ATTRTYPE_QWORD:
	return sizeof(unsigned long long);

    case ATTRTYPE_FLOAT:
	return sizeof(float);

    case ATTRTYPE_DOUBLE:
	return sizeof(double);

    case ATTRTYPE_STRING:
	assert(_string);
	return sizeof(unsigned char) + static_cast<unsigned int>(_string->size());

    case ATTRTYPE_LONGSTRING:
	assert(_string);
	return sizeof(unsigned int) + static_cast<unsigned int>(_string->size());
    }
    assert(0);
    return 0;
}

bool AnyType::setInteger(int i)
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	_int = (i != 0) ? 1 : 0;
	return true;

    case ATTRTYPE_CHAR:
	if (SCHAR_MIN <= i and i <= SCHAR_MAX) {
	    _int = i;
	    return true;
	}
	if (SCHAR_MIN > i) _int = SCHAR_MIN;
	if (i > SCHAR_MAX) _int = SCHAR_MAX;
	return false;

    case ATTRTYPE_BYTE:
	if (i <= UCHAR_MAX) {
	    _uint = static_cast<unsigned char>(i);
	    return true;
	}
	if (0 > i) _uint = 0;
	if (i > UCHAR_MAX) _uint = UCHAR_MAX;
	return false;

    case ATTRTYPE_SHORT:
	if (SHRT_MIN <= i and i <= SHRT_MAX) {
	    _int = i;
	    return true;
	}
	if (SHRT_MIN > i) _int = SHRT_MIN;
	if (i > SHRT_MAX) _int = SHRT_MAX;
	return false;

    case ATTRTYPE_WORD:
	if (i <= USHRT_MAX) {
	    _uint = static_cast<unsigned short>(i);
	    return true;
	}
	if (0 > i) _uint = 0;
	if (i > USHRT_MAX) _uint = USHRT_MAX;
	return false;

    case ATTRTYPE_INTEGER:
	if (INT_MIN <= i and i <= INT_MAX) {
	    _int = i;
	    return true;
	}
	if (INT_MIN > i) _int = INT_MIN;
	if (i > INT_MAX) _int = INT_MAX;
	return false;

    case ATTRTYPE_DWORD:
	if (static_cast<unsigned int>(i) <= UINT_MAX) {
	    _uint = i;
	    return true;
	}
	if (0 > i) _uint = 0;
	return false;

    case ATTRTYPE_LONG:
	_long = i;
	return true;

    case ATTRTYPE_QWORD:
	_ulong = i;
	return true;

    case ATTRTYPE_FLOAT:
	_float = static_cast<float>(i);
	return true;

    case ATTRTYPE_DOUBLE:
	_double = static_cast<double>(i);
	return true;

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	char buf[32];
	assert(_string);
	*_string = convert_signed_decimal(i, buf, sizeof(buf));
	return true;
    }
    }
    assert(0);
    return false;
}

bool AnyType::setLong(long long l)
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	_int = (l != 0) ? 1 : 0;
	return true;

    case ATTRTYPE_CHAR:
	if (SCHAR_MIN <= l and l <= SCHAR_MAX) {
	    _int = static_cast<char>(l);
	    return true;
	}
	if (SCHAR_MIN > l) _int = SCHAR_MIN;
	if (l > SCHAR_MAX) _int = SCHAR_MAX;
	return false;

    case ATTRTYPE_BYTE:
	if (0 <= l and l <= UCHAR_MAX) {
	    _uint = static_cast<unsigned char>(l);
	    return true;
	}
	if (0 > l) _uint = 0;
	if (l > UCHAR_MAX) _uint = UCHAR_MAX;
	return false;

    case ATTRTYPE_SHORT:
	if (SHRT_MIN <= l and l <= SHRT_MAX) {
	    _int = static_cast<short>(l);
	    return true;
	}
	if (SHRT_MIN > l) _int = SHRT_MIN;
	if (l > SHRT_MAX) _int = SHRT_MAX;
	return false;

    case ATTRTYPE_WORD:
	if (l <= USHRT_MAX) {
	    _uint = static_cast<unsigned short>(l);
	    return true;
	}
	if (0 > l) _uint = 0;
	if (l > USHRT_MAX) _uint = USHRT_MAX;
	return false;

    case ATTRTYPE_INTEGER:
	if (INT_MIN <= l and l <= INT_MAX) {
	    _int = l;
	    return true;
	}
	if (INT_MIN > l) _int = INT_MIN;
	if (l > INT_MAX) _int = INT_MAX;
	return false;

    case ATTRTYPE_DWORD:
	if (static_cast<unsigned int>(l) <= UINT_MAX) {
	    _uint = l;
	    return true;
	}
	if (0 > l) _uint = 0;
	if (l > UINT_MAX) _uint = UINT_MAX;
	return false;

    case ATTRTYPE_LONG:
	_long = l;
	return true;

    case ATTRTYPE_QWORD:
	_ulong = l;
	return true;

    case ATTRTYPE_FLOAT:
	_float = static_cast<float>(l);
	return true;

    case ATTRTYPE_DOUBLE:
	_double = static_cast<double>(l);
	return true;

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	char buf[32];
	assert(_string);
	*_string = convert_signed64_decimal(l, buf, sizeof(buf));
	return true;
    }
    }
    assert(0);
    return false;
}

bool AnyType::setDouble(double d)
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	if (0 <= d and d <= 0.5) {
	    _int = 0;
	    return true;
	}
	if (0.5 < d and d <= 1) {
	    _int = 1;
	    return true;
	}
	return false;

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return setInteger( static_cast<int>(d) );

    case ATTRTYPE_LONG:
	return setLong( static_cast<long long>(d) );

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return setInteger( static_cast<unsigned int>(d) );

    case ATTRTYPE_QWORD:
	return setLong( static_cast<unsigned long long>(d) );

    case ATTRTYPE_FLOAT:
	_float = static_cast<float>(d);
	return true;

    case ATTRTYPE_DOUBLE:
	_double = d;
	return true;

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	char buf[64];
	assert(_string);
	g_snprintf(buf, sizeof(buf), "%.2f", d);
	*_string = buf;
	return true;
    }
    }
    assert(0);
    return false;
}

bool AnyType::setString(const std::string &s)
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	if (s == "0" or s == "f" or s == "false" or s == "n" or s == "no") {
	    _int = 0;
	    return true;
	}
	if (s == "1" or s == "t" or s == "true" or s == "y" or s == "yes") {
	    _int = 1;
	    return true;
	}
	return false;

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    {
	char *endptr;
	long i = strtol(s.c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return setInteger(i);
	return false;
    }

    case ATTRTYPE_LONG:
    {
	char *endptr;
	long long l = strtoll(s.c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return setLong(l);
	return false;
    }

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    {
	char *endptr;
	unsigned long u = strtoul(s.c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return setInteger(u);
	return false;
    }
    
    case ATTRTYPE_QWORD:
    {
	char *endptr;
	unsigned long long u = strtoull(s.c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return setLong(u);
	return false;
    }

    case ATTRTYPE_FLOAT:
    {
	char *endptr;
	float f = static_cast<float>(strtod(s.c_str(), &endptr));
	if (endptr != NULL and *endptr == 0) {
	    _float = f;
	    return true;
	}
	return false;
    }

    case ATTRTYPE_DOUBLE:
    {
	char *endptr;
	double d = strtod(s.c_str(), &endptr);
	if (endptr != NULL and *endptr == 0) {
	    _double = d;
	    return true;
	}
	return false;
    }

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	*_string = s;
	return true;
    }
    }
    assert(0);
    return false;
}

bool AnyType::setStringQuoted(const std::string &s)
{
    // unescape string
    if (s.size() < 2) return false;
    if (s[0] != '"') return false;
    if (s[s.size()-1] != '"') return false;

    std::string t;
    t.reserve(s.size() + s.size() / 32);

    for(unsigned int i = 1; i + 1 < s.size(); i++)
    {
	if (s[i] == '\\')
	{
	    i++;
	    if (i >= s.size() - 1) return false;

	    switch(s[i])
	    {
	    case 'a':	t += '\a';	break;
	    case 'b':	t += '\b';	break;
	    case 'f':	t += '\f';	break;
	    case 'n':	t += '\n';	break;
	    case 'r':	t += '\r';	break;
	    case 't':	t += '\t';	break;
	    case 'v':	t += '\v';	break;
	    case '\'':	t += '\'';	break;
	    case '"':	t += '"';	break;
	    case '\\':	t += '\\';	break;
	    case '?':	t += '?';	break;

	    default:
		t += '\\';
		t += s[i];
		break;
	    }
	}
	else {
	    t += s[i];
	}
    }

    return setString(t);
}

bool AnyType::getBoolean() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	return (_int != 0);

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return (_int != 0);

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return (_uint != 0);

    case ATTRTYPE_LONG:
	return (_long != 0);

    case ATTRTYPE_QWORD:
	return (_ulong != 0);

    case ATTRTYPE_FLOAT:
	return (_float != 0.0f);

    case ATTRTYPE_DOUBLE:
	return (_double != 0.0f);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);

	if (*_string == "0" or *_string == "f" or *_string == "false"
	    or *_string == "n" or *_string == "no") {
	    return false;
	}
	if (*_string == "1" or *_string == "t" or *_string == "true"
	    or *_string == "y" or *_string == "yes") {
	    return true;
	}

	throw(ConversionException("Cannot convert string to bool."));
    }
    }
    assert(0);
    return false;
}

int AnyType::getInteger() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	return _int;

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return _int;

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return _uint;

    case ATTRTYPE_LONG:
	if (_long > INT_MAX) return INT_MAX;
	if (_long < INT_MIN) return INT_MIN;

	return static_cast<int>(_long);

    case ATTRTYPE_QWORD:
	if (_ulong > UINT_MAX) return UINT_MAX;
	return static_cast<int>(_ulong);

    case ATTRTYPE_FLOAT:
	return static_cast<int>(_float);

    case ATTRTYPE_DOUBLE:
	return static_cast<int>(_double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	char *endptr;
	long i = strtol(_string->c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return i;

	throw(ConversionException("Cannot convert string to integer."));
    }
    }
    assert(0);
    return false;
}

unsigned int AnyType::getUnsignedInteger() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return static_cast<unsigned int>(_int);

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return _uint;

    case ATTRTYPE_LONG:
	return static_cast<unsigned int>(_long);

    case ATTRTYPE_QWORD:
	return static_cast<unsigned int>(_ulong);

    case ATTRTYPE_FLOAT:
	return static_cast<unsigned int>(_float);

    case ATTRTYPE_DOUBLE:
	return static_cast<unsigned int>(_double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	char *endptr;
	unsigned long i = strtoul(_string->c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return i;

	throw(ConversionException("Cannot convert string to unsigned integer."));
    }
    }
    assert(0);
    return false;
}

long long AnyType::getLong() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return static_cast<long long>(_int);

    case ATTRTYPE_LONG:
	return _long;

    case ATTRTYPE_QWORD:
	return _ulong;

    case ATTRTYPE_FLOAT:
	return static_cast<long long>(_float);

    case ATTRTYPE_DOUBLE:
	return static_cast<long long>(_double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	char *endptr;
	long long l = strtoll(_string->c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return l;

	throw(ConversionException("Cannot convert string to long long."));
    }
    }
    assert(0);
    return false;
}

unsigned long long AnyType::getUnsignedLong() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	return static_cast<unsigned long long>(_int);

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return static_cast<unsigned long long>(_uint);

    case ATTRTYPE_LONG:
	return static_cast<unsigned long long>(_long);

    case ATTRTYPE_QWORD:
	return _ulong;

    case ATTRTYPE_FLOAT:
	return static_cast<unsigned long long>(_float);

    case ATTRTYPE_DOUBLE:
	return static_cast<unsigned long long>(_double);

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	char *endptr;
	unsigned long long l = strtoul(_string->c_str(), &endptr, 10);
	if (endptr != NULL and *endptr == 0)
	    return l;

	throw(ConversionException("Cannot convert string to unsigned long long."));
    }
    }
    assert(0);
    return false;
}

double AnyType::getDouble() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	return static_cast<double>(_int);

    case ATTRTYPE_FLOAT:
	return static_cast<double>(_float);

    case ATTRTYPE_LONG:
	return static_cast<double>(_long);

    case ATTRTYPE_QWORD:
	return static_cast<double>(_ulong);

    case ATTRTYPE_DOUBLE:
	return _double;

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	char *endptr;
	double d = strtod(_string->c_str(), &endptr);
	if (endptr != NULL and *endptr == 0)
	    return d;

	throw(ConversionException("Cannot convert string to double."));
    }
    }
    assert(0);
    return false;
}

std::string AnyType::getString() const
{
    switch(type)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
	if (_int == 0) return "false";
	if (_int == 1) return "true";
	return "invalid";

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    {
	char buf[32];
	return convert_signed_decimal(_int, buf, sizeof(buf));
    }

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    {
	char buf[32];
	return convert_unsigned_decimal(_uint, buf, sizeof(buf));
    }

    case ATTRTYPE_LONG:
    {
	char buf[64];
	return convert_signed64_decimal(_long, buf, sizeof(buf));
    }

    case ATTRTYPE_QWORD:
    {
	char buf[64];
	return convert_unsigned64_decimal(_ulong, buf, sizeof(buf));
    }

    case ATTRTYPE_FLOAT:
    {
	char buf[32];
	g_snprintf(buf, sizeof(buf), "%.2f", _float);
	return buf;
    }

    case ATTRTYPE_DOUBLE:
    {
	char buf[32];
	g_snprintf(buf, sizeof(buf), "%.2f", _double);
	return buf;
    }

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	assert(_string);
	return *_string;
    } 
    }
    assert(0);
    return false;
}

void AnyType::resetType(attrtype_t t)
{
    // if setting to a string and this is not a string, create the object
    if (t == ATTRTYPE_STRING or t == ATTRTYPE_LONGSTRING) {
	if (type != ATTRTYPE_STRING and t != ATTRTYPE_LONGSTRING) {
	    _string = new std::string;
	}
    }
    else {
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    delete _string;
	}

	_double = 0;
    }

    type = t;
}

bool AnyType::convertType(attrtype_t t)
{
    // same source and target type?
    if (type == t) return true;

    // special case if this object was initialised with the ATTRTYPE_INVALID:
    // all the get...() below would assert.
    if (type == ATTRTYPE_INVALID)
    {
	type = t;
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    _string = new std::string;
	}
	return true;
    }
    
    switch(t)
    {
    case ATTRTYPE_INVALID:
	assert(0);
	return false;

    case ATTRTYPE_BOOL:
    {
	bool v = getBoolean();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_int = v;
	type = t;
	return true;
    }

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    {
	int v = getInteger();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_int = v;
	type = t;
	return true;
    }

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    {
	unsigned int v = getUnsignedInteger();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_uint = v;
	type = t;
	return true;
    }

    case ATTRTYPE_LONG:
    {
	long long v = getLong();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_long = v;
	type = t;
	return true;
    }

    case ATTRTYPE_QWORD:
    {
	unsigned long long v = getLong();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_ulong = v;
	type = t;
	return true;
    }

    case ATTRTYPE_FLOAT:
    {
	float f = static_cast<float>(getDouble());
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_float = f;
	type = t;
	return true;
    }

    case ATTRTYPE_DOUBLE:
    {
	double d = getDouble();
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) delete _string;
	_double = d;
	type = t;
	return true;
    }

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	// dont allocate a new string in this case
	if (type == ATTRTYPE_STRING or type == ATTRTYPE_LONGSTRING) {
	    type = t;
	    return true;
	}

	_string = new std::string(getString());
	type = t;
	return true;
    } 
    }
    assert(0);
    return false;
}

/// unary prefix - operator. Converts the AnyType to a numeric value. Bool
/// values are inverted. Unsigned Types are handled just like signed types,
/// even tho it doesnt make sense.
AnyType AnyType::operator-() const
{
    AnyType at = *this;
    
    switch(at.type)
    {
    case ATTRTYPE_INVALID: assert(0); throw(ConversionException("Negating invalid type."));
	
    case ATTRTYPE_BOOL:
	if (at._int > 0) at._int = 0;
	else at._int = 1;
	break;

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
	at._int = - at._int;
	break;

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
	at._uint = - at._uint;
	break;

    case ATTRTYPE_LONG:
	at._long = - at._long;
	break;

    case ATTRTYPE_QWORD:
	at._ulong = - at._ulong;
	break;

    case ATTRTYPE_FLOAT:
	at._float = - at._float;
	break;

    case ATTRTYPE_DOUBLE:
	at._double = - at._double;
	break;

    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	if (not at.convertType(ATTRTYPE_DOUBLE))
	    throw(ConversionException("Cannot convert string to double for negation."));

	at._double = - at._double;
	break;
    }
    }

    return at;
}

/// Binary arithmatic template operator. Converts the two AnyTypes into the
/// largest type of their common field. If a string cannot be converted to a
/// numeric of the same field as the other operand a ConversionException is
/// thrown. Bool types cannot be added.
template <template <typename Type> class Operator, char OpName>
AnyType AnyType::binary_arith_op(const AnyType &b) const
{
    switch(type)
    {
    case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for first operand of binary operator ")+OpName+"."));

    case ATTRTYPE_BOOL:
	throw(ConversionException("No binary operators are allowed on bool values."));

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return AnyType( op(_int, b._int) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<int> op;
	    return AnyType( op(_int, static_cast<signed int>(b._uint)) );
	}

	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return AnyType( op(_int, b._long) );
	}
	    
	case ATTRTYPE_QWORD:
	{
	    Operator<long long> op;
	    return AnyType( op(static_cast<long long>(_int), static_cast<long long>(b._ulong)) );
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(static_cast<float>(_int), b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(static_cast<double>(_int), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_INTEGER))
		throw(ConversionException(std::string("Could not convert string to integer for binary operator ")+OpName+"."));

	    Operator<int> op;

	    return AnyType( op(_int, bc.getInteger()) );
	}
	}
	break;
    }

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return AnyType( op(static_cast<signed int>(_uint), b._int) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned int> op;
	    return AnyType( op(_uint, b._uint) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return AnyType( op(static_cast<signed long long>(_uint), b._long) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return AnyType( op(static_cast<unsigned long long>(_uint), b._ulong) );
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(static_cast<float>(_uint), b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(static_cast<double>(_uint), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_DWORD))
		throw(ConversionException(std::string("Could not convert string to unsigned integer for binary operator ")+OpName+"."));

	    Operator<unsigned int> op;

	    return AnyType( op(_int, bc.getInteger()) );
	}
	}
	break;
    }

    case ATTRTYPE_LONG:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<long long> op;
	    return AnyType( op(_long, static_cast<long long>(b._int)) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<long long> op;
	    return AnyType( op(_long, static_cast<signed long long>(b._uint)) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return AnyType( op(_long, b._long) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<long long> op;
	    return AnyType( op(_long, static_cast<signed long long>(b._ulong)) );
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(static_cast<float>(_long), b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(static_cast<double>(_long), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_LONG))
		throw(ConversionException(std::string("Could not convert string to long for binary operator ")+OpName+"."));

	    Operator<long long> op;

	    return AnyType( op(_long, bc.getLong()) );
	}
	}
	break;
    }

    case ATTRTYPE_QWORD:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<long long> op;
	    return AnyType( op(static_cast<signed long long>(_ulong), b._int) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned long long> op;
	    return AnyType( op(_ulong, static_cast<unsigned long long>(b._uint)) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return AnyType( op(static_cast<signed long long>(_ulong), b._long) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return AnyType( op(_ulong, b._ulong) );
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(static_cast<float>(_ulong), b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(static_cast<double>(_ulong), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_QWORD))
		throw(ConversionException(std::string("Could not convert string to unsigned long long for binary operator ")+OpName+"."));

	    Operator<unsigned long long> op;

	    return AnyType( op(_ulong, bc.getLong()) );
	}
	}
	break;
    }

    case ATTRTYPE_FLOAT:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<float> op;
	    return AnyType( op(_float, static_cast<float>(b._int)) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<float> op;
	    return AnyType( op(_float, static_cast<float>(b._uint)) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<float> op;
	    return AnyType( op(_float, static_cast<float>(b._long)) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<float> op;
	    return AnyType( op(_float, static_cast<float>(b._ulong)) );
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(_float, b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(static_cast<double>(_float), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_FLOAT))
		throw(ConversionException(std::string("Could not convert string to float for binary operator ")+OpName+"."));

	    Operator<float> op;

	    return AnyType( op(_float, bc._float) );
	}
	}

	break;
    }

    case ATTRTYPE_DOUBLE:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<double> op;
	    return AnyType( op(_double, static_cast<double>(b._int)) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<double> op;
	    return AnyType( op(_double, static_cast<double>(b._uint)) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<double> op;
	    return AnyType( op(_double, static_cast<double>(b._long)) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<double> op;
	    return AnyType( op(_double, static_cast<double>(b._ulong)) );
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<double> op;
	    return AnyType( op(_double, static_cast<double>(b._float)));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(_double, b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_DOUBLE))
		throw(ConversionException(std::string("Could not convert string to double for binary operator ")+OpName+"."));

	    Operator<double> op;

	    return AnyType( op(_double, bc.getDouble()) );
	}
	}

	break;
    }

    // if this is a string, then the other type defines the conversion
    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpName+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpName+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return AnyType( op(this->getInteger(), b._int) );
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned int> op;
	    return AnyType( op(this->getUnsignedInteger(), b._int) );
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return AnyType( op(this->getLong(), b._long) );
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return AnyType( op(this->getUnsignedLong(), b._ulong) );
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return AnyType( op(static_cast<float>(this->getDouble()), b._float) );
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return AnyType( op(this->getDouble(), b._double) );
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	    throw(ConversionException("No binary operators are allowed between two string values."));
	}
	break;
    }
    }
    assert(0);
    throw(ConversionException("Invalid binary operator call. (PROGRAM ERROR)"));
}

// *** Instanciate binary_op for the four arithmetic operators
template AnyType AnyType::binary_arith_op<std::plus, '+'>(const AnyType &b) const;
template AnyType AnyType::binary_arith_op<std::minus, '-'>(const AnyType &b) const;
template AnyType AnyType::binary_arith_op<std::multiplies, '*'>(const AnyType &b) const;
template AnyType AnyType::binary_arith_op<std::divides, '/'>(const AnyType &b) const;

/// Binary comparison template operator. Converts the two AnyTypes into the
/// largest type of their common field. If a string cannot be converted to a
/// numeric of the same field as the other operand a ConversionException is
/// thrown. Bool types cannot be added.
template <template <typename Type> class Operator, int OpNum>
bool AnyType::binary_comp_op(const AnyType &b) const
{
    static const char *OpNameArray[] = { "==", "!=", "<", ">", "<=", ">=" };

    switch(type)
    {
    case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for first operand of binary operator ")+OpNameArray[OpNum]+"."));

    case ATTRTYPE_BOOL:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL:
	{
	    Operator<bool> op;
	    return op(_int, b._int);
	}

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return op(_int, b._int);
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<int> op;
	    return op(_int, static_cast<signed int>(b._uint));
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(_int, b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<long long> op;
	    return op(_int, static_cast<signed long long>(b._ulong));
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(_int), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_int), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_BOOL))
		throw(ConversionException(std::string("Could not convert string to bool for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<int> op;

	    return op(_int, bc.getInteger());
	}
	}
	break;
    }

    case ATTRTYPE_CHAR:
    case ATTRTYPE_SHORT:
    case ATTRTYPE_INTEGER:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return op(_int, b._int);
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<int> op;
	    return op(_int, static_cast<signed int>(b._uint));
	}

	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(_int, b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<long long> op;
	    return op(_int, static_cast<signed long long>(b._ulong));
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(_int), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_int), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_INTEGER))
		throw(ConversionException(std::string("Could not convert string to integer for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<int> op;

	    return op(_int, bc.getInteger());
	}
	}
	break;
    }

    case ATTRTYPE_BYTE:
    case ATTRTYPE_WORD:
    case ATTRTYPE_DWORD:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return op(static_cast<signed int>(_uint), b._int);
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned int> op;
	    return op(_uint, b._uint);
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(static_cast<signed long long>(_uint), b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return op(static_cast<unsigned long long>(_uint), b._ulong);
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(_uint), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_uint), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_DWORD))
		throw(ConversionException(std::string("Could not convert string to unsigned integer for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<unsigned int> op;

	    return op(_int, bc.getInteger());
	}
	}
	break;
    }

    case ATTRTYPE_LONG:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<long long> op;
	    return op(_long, static_cast<long long>(b._int));
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<long long> op;
	    return op(_long, static_cast<signed long long>(b._uint));
	}

	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(_long, b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<long long> op;
	    return op(_long, static_cast<signed long long>(b._ulong));
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(_long), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_long), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_LONG))
		throw(ConversionException(std::string("Could not convert string to long long for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<long long> op;

	    return op(_long, bc.getLong());
	}
	}
	break;
    }

    case ATTRTYPE_QWORD:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<long long> op;
	    return op(_ulong, static_cast<signed long long>(b._int));
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned long long> op;
	    return op(_ulong, b._uint);
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(static_cast<signed long long>(_ulong), b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return op(_ulong, b._ulong);
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(_ulong), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_ulong), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_QWORD))
		throw(ConversionException(std::string("Could not convert string to unsigned long long for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<unsigned long long> op;

	    return op(_int, bc.getUnsignedLong());
	}
	}
	break;
    }

    case ATTRTYPE_FLOAT:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<float> op;
	    return op(_float, static_cast<float>(b._int));
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<float> op;
	    return op(_float, static_cast<float>(b._uint));
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<float> op;
	    return op(_float, static_cast<float>(b._long));
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<float> op;
	    return op(_float, static_cast<float>(b._ulong));
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(_float, b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(static_cast<double>(_float), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_FLOAT))
		throw(ConversionException(std::string("Could not convert string to float for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<float> op;

	    return op(_float, bc._float);
	}
	}

	break;
    }

    case ATTRTYPE_DOUBLE:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<double> op;
	    return op(_double, static_cast<double>(b._int));
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<double> op;
	    return op(_double, static_cast<double>(b._uint));
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<double> op;
	    return op(_double, static_cast<double>(b._long));
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<double> op;
	    return op(_double, static_cast<double>(b._ulong));
	}
	    
	case ATTRTYPE_FLOAT:
	{
	    Operator<double> op;
	    return op(_double, static_cast<double>(b._float));
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(_double, b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	{
	    AnyType bc = b;
	    
	    if (not bc.convertType(ATTRTYPE_DOUBLE))
		throw(ConversionException(std::string("Could not convert string to double for binary operator ")+OpNameArray[OpNum]+"."));

	    Operator<double> op;

	    return op(_double, bc.getDouble());
	}
	}

	break;
    }

    // if this is a string, then the other type defines the conversion
    case ATTRTYPE_STRING:
    case ATTRTYPE_LONGSTRING:
    {
	switch(b.type)
	{
	case ATTRTYPE_INVALID: assert(0); throw(ConversionException(std::string("Invalid type for second operand of binary operator ")+OpNameArray[OpNum]+"."));

	case ATTRTYPE_BOOL: throw(ConversionException(std::string("Binary operator ")+OpNameArray[OpNum]+" is not permitted on bool values."));

	case ATTRTYPE_CHAR:
	case ATTRTYPE_SHORT:
	case ATTRTYPE_INTEGER:
	{
	    Operator<int> op;
	    return op(this->getInteger(), b._int);
	}

	case ATTRTYPE_BYTE:
	case ATTRTYPE_WORD:
	case ATTRTYPE_DWORD:
	{
	    Operator<unsigned int> op;
	    return op(this->getUnsignedInteger(), b._int);
	}
	    
	case ATTRTYPE_LONG:
	{
	    Operator<long long> op;
	    return op(this->getLong(), b._long);
	}

	case ATTRTYPE_QWORD:
	{
	    Operator<unsigned long long> op;
	    return op(this->getUnsignedLong(), b._ulong);
	}

	case ATTRTYPE_FLOAT:
	{
	    Operator<float> op;
	    return op(static_cast<float>(this->getDouble()), b._float);
	}

	case ATTRTYPE_DOUBLE:
	{
	    Operator<double> op;
	    return op(this->getDouble(), b._double);
	}

	case ATTRTYPE_STRING:
	case ATTRTYPE_LONGSTRING:
	    throw(ConversionException("No binary operators are allowed between two string values."));
	}
	break;
    }
    }
    assert(0);
    throw(ConversionException("Invalid binary operator call. (PROGRAM ERROR)"));
}

template bool AnyType::binary_comp_op<std::equal_to, 0>(const AnyType &b) const;
template bool AnyType::binary_comp_op<std::not_equal_to, 1>(const AnyType &b) const;
template bool AnyType::binary_comp_op<std::less, 2>(const AnyType &b) const;
template bool AnyType::binary_comp_op<std::greater, 3>(const AnyType &b) const;
template bool AnyType::binary_comp_op<std::less_equal, 4>(const AnyType &b) const;
template bool AnyType::binary_comp_op<std::greater_equal, 5>(const AnyType &b) const;

} // namespace VGServer
