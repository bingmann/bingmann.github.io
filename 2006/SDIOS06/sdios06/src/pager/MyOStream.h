// $Id: MyOStream.h 155 2006-07-24 20:21:28Z sdi2 $

#ifndef SDI2_PAGER_MYOSTREAM_H
#define SDI2_PAGER_MYOSTREAM_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sdi/util.h>

namespace mystd {

/** ostringstream is a static allocated string buffer in memory to serve virtual
 * files via the name service and file interface. */

class ostringstream
{
private:

	static const size_t	datalen = 4096;

	char 			data[datalen];

	size_t		 	end;

public:
	int				base;

	/// empty string
	inline ostringstream()
	{
		base = 10;
		end = 0;
	}

	/// clear
	inline void clear()
	{
		end = 0;
	}

	/// well. print the string.
	inline void dump()
	{
		assert(end < datalen-1);
		data[end] = 0;
		printf("ostringstream (%u bytes):\n%s\n", end, data);
	}

	// append len bytes to the end of the data
	inline void append(const char* str, size_t len)
	{
		memcpy(data + end, str, min(datalen - end - 1, len));
		end += min(datalen - end - 1, len);
	}

	/// guess
	inline size_t size() const
	{ return end; }

	// interface to satisfy random reads from IF_FILE
	inline size_t read(size_t pos, void *dest, size_t len) const
	{
		if (pos >= end) return 0;

		len = min(len, end - pos);
		memcpy(dest, data + pos, len);

		return len;
	}
};

// nice typesafe interface

static inline ostringstream& operator<< (ostringstream& out, const char* str)
{
	out.append(str, strlen(str));
	return out;
}

static inline ostringstream& operator<<(ostringstream& out, long v)
{
	char buf[20];
	switch(out.base)
	{
	case 8:
		snprintf(buf, sizeof(buf), "0%lo", v);
		break;
	default:
	case 10:
		snprintf(buf, sizeof(buf), "%ld", v);
		break;
	case 16:
		snprintf(buf, sizeof(buf), "0x%lx", v);
		break;
	}
	return out << buf;
}	

static inline ostringstream& operator<<(ostringstream& out, unsigned long v)
{
	char buf[20];
	switch(out.base)
	{
	case 8:
		snprintf(buf, sizeof(buf), "0%lo", v);
		break;
	default:
	case 10:
		snprintf(buf, sizeof(buf), "%lu", v);
		break;
	case 16:
		snprintf(buf, sizeof(buf), "0x%lx", v);
		break;
	}
	return out << buf;
}			

static inline ostringstream& operator<<(ostringstream& out, int v)
{
	return out << static_cast<long>(v);
}
	
static inline ostringstream& operator<<(ostringstream& out, unsigned int v)
{
	return out << static_cast<unsigned long>(v);
}
	
static inline ostringstream& operator<<(ostringstream& out, float v)
{
	char buf[30];
	snprintf(buf, sizeof(buf), "%f", v);
	return out << buf;
}

// object to send to <<
struct ios_setbase {
	int base;

	ios_setbase(int b)
		: base(b)
	{
	}
};

inline ios_setbase setbase(int b)
{
	return ios_setbase(b);
}

static inline ostringstream& operator<<(ostringstream& out, ios_setbase set)
{
	out.base = set.base;
	return out;
}

static const ios_setbase oct(8);
static const ios_setbase dec(10);
static const ios_setbase hex(16);

} // namespace mystd

#endif // SDI2_PAGER_MYOSTREAM_H
