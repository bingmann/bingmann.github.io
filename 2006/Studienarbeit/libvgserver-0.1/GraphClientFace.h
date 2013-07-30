// $Id: GraphClientFace.h 127 2006-05-13 15:30:07Z bingmann $

#ifndef VGS_GraphClientFace_H
#define VGS_GraphClientFace_H

#include "GraphConnection.h"
#include "ByteOutBuffer.h"

namespace VGServer {

/** GraphClientFace is a facade class for the client to strip down the
 * functionality of GraphConnection. */

class GraphClientFace
{
private:
    class GraphConnection& conn;

public:
    GraphClientFace(class GraphConnection &_conn)
	: conn(_conn)
    {
    }
    
    /// main function for the client: retrieve all vertices and edges within
    /// the specified rectangle which match the expression in filter. Includes
    /// the attributes which's names are in vertexattrsel or edgeattrsel.
    void	getArea(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
			const QueryLimits *ql,
			const char* filter, const char* vertexattrsel, const char* edgeattrsel,
			class ByteBuffer &dest) const
    {
	conn.getArea(x1, y1, x2, y2, ql, filter, vertexattrsel, edgeattrsel, dest);
    }    

    /// the only other server-driven lookup function currently designed. Useful
    /// for mouse clicks. Or should this be implemented on client side using
    /// it's local graph snapshot?
    //unsigned int getNearestVertexId(coord_t x, coord_t y) const;
};

} // namespace VGServer

#endif // VGS_GraphClientFace_H
