// $Id: GraphConnection.cc 127 2006-05-13 15:30:07Z bingmann $

#include "GraphConnection.h"
#include "GraphContainer.h"
#include "GraphClientFace.h"
#include "ByteOutBuffer.h"

namespace VGServer {

GraphConnection::GraphConnection(GraphContainer &_graph)
    : graph(_graph), changes(graph)
{
}

GraphConnection::~GraphConnection()
{
}

class GraphClientFace GraphConnection::getClientFace()
{ return GraphClientFace(*this); }

void GraphConnection::rollback()
{
    changes.clear();
}

void GraphConnection::commit()
{
    graph.applyChangelist(changes.getChangelistEnd());
    changes.clear();
}

void GraphConnection::getGraphProperties(class ByteBuffer &dest) const
{
    graph.getGraphProperties(dest);
}

void
GraphConnection::getArea(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
			 const QueryLimits *ql,
			 const char* filter, const char* vertexattrsel, const char* edgeattrsel,
			 class ByteBuffer &dest) const
{
    graph.getArea(x1, y1, x2, y2, ql, filter, vertexattrsel, edgeattrsel, changes, dest);
}

void 
GraphConnection::getNearestNeighbor(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
				    coord_t selx, coord_t sely, const QueryLimits *ql,
				    const char* filter, const char* vertexattrsel, const char* edgeattrsel,
				    class ByteBuffer &dest) const
{
    graph.getNearestNeighbor(x1, y1, x2, y2, selx, sely, ql, filter, vertexattrsel, edgeattrsel, changes.getChangelistStart(), dest);
}

} // namespace VGServer
