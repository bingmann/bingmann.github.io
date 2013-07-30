// $Id: GraphTypes.h 165 2006-05-29 16:39:53Z bingmann $

#ifndef VGS_GraphTypes_H
#define VGS_GraphTypes_H

#include <limits.h>
#include <vector>
#include <string>

/// Library VGServer primary namespace
namespace VGServer {

/** \defgroup Public Public Application Interface
 *  \defgroup Exception Exceptions thrown by various parts of the library
 *  \defgroup Private Private Implementation Details
*/

/// this is the type of the world coordinates
typedef int coord_t;

/// illegal value of coordinates: used for deleted vertices
const coord_t COORD_INVALID = INT_MAX;

/// index type used for the vertices
typedef unsigned int vertexid_t;

/// invalid vertex id, used for filling up edge arrays
const vertexid_t VERTEX_INVALID = UINT_MAX;

/// enumeration used in some places where function operate on either vertices
/// or edges.
enum vertex_or_edge_t { VE_VERTEX=0, VE_EDGE=1 };

/// attrtype_t is the enum type used for the different attribute types
enum attrtype_t
{ ATTRTYPE_INVALID = 0x0,
  ATTRTYPE_BOOL = 0x1,
  ATTRTYPE_CHAR = 0x10,   ATTRTYPE_SHORT = 0x11, ATTRTYPE_INTEGER = 0x12, ATTRTYPE_LONG = 0x13,
  ATTRTYPE_BYTE = 0x20,   ATTRTYPE_WORD = 0x21,  ATTRTYPE_DWORD = 0x22, ATTRTYPE_QWORD = 0x23,
  ATTRTYPE_FLOAT = 0x30,  ATTRTYPE_DOUBLE = 0x31,
  ATTRTYPE_STRING = 0x40, ATTRTYPE_LONGSTRING = 0x41
};

/// this is an evil macro used in switched to make one block for all
/// fixed-length attribute types. The last : is omitted!
#define CASE_FIXED_LENGTH	\
case ATTRTYPE_BOOL: \
case ATTRTYPE_CHAR: case ATTRTYPE_SHORT: case ATTRTYPE_INTEGER: case ATTRTYPE_LONG: \
case ATTRTYPE_BYTE: case ATTRTYPE_WORD: case ATTRTYPE_DWORD: case ATTRTYPE_QWORD: \
case ATTRTYPE_FLOAT: case ATTRTYPE_DOUBLE

/** This struct contains the four limit parameters of getArea and
 * getNearestNeighbor */
struct QueryLimits
{
    unsigned int vertexmaxlimit;
    unsigned int vertexminlimit;
    unsigned int edgemaxlimit;
    unsigned int edgeminlimit;

    inline QueryLimits() {
	vertexmaxlimit = vertexminlimit = UINT_MAX;
	edgemaxlimit = edgeminlimit = UINT_MAX;
    }

    inline QueryLimits(unsigned int vxl, unsigned int vnl,
		       unsigned int exl, unsigned int enl)
	: vertexmaxlimit(vxl), vertexminlimit(vnl),
	  edgemaxlimit(exl), edgeminlimit(enl)
    {
    }
};

/** GraphException is the base class for exceptions by some part of the server
 * library. \ingroup Exception */

class GraphException : public std::exception
{
private:
    std::string	msg;

public:
    inline GraphException(const std::string &s) throw()
	: std::exception(), msg(s)
    { }

    virtual ~GraphException() throw()
    { }

    virtual const char* what() throw()
    { return msg.c_str(); }

    inline const std::string& what_str() throw()
    { return msg; }
};

} // namespace VGServer

#endif // VGS_GraphTypes_H
