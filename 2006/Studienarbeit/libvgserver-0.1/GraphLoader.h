// $Id: GraphLoader.h 244 2006-07-07 09:11:51Z schultes $

#ifndef VGS_GraphLoader_H
#define VGS_GraphLoader_H

#include "GraphTypes.h"
#include "GraphData.h"

namespace VGServer {

/** GraphLoader is a class which extends GraphData by load functions which
 * accept vertex and edge data _in_ascending_order_ and write it into a global
 * GraphData's structure. \ingroup Public */

class GraphLoader : private GraphData
{
private:
    // *** Vertex Attribute Loading Sequence

    /// minimum next vertex id in the vertex attribute loading sequence
    /// => +1 of the last vertex loaded
    vertexid_t		vertex_minnum;

    /// index into the attribute array, points to default bitfield of the
    /// current vertex id.
    unsigned int	vertex_aidx;

    /// attribute array index pointing to next byte to be filled.
    unsigned int	vertex_aidxnext;

    /// index of the last attribute id loaded
    unsigned int	vertex_lastid;

    /// fill in the remaining attribute values of the current vertex
    void                finishVertexAttrSequence();
    
    
    /// *** Edge Attribute Loading Sequence

    /// minimum next source vertex id in the edge attribute loading sequence 
    vertexid_t		edge_minsrc;

    /// minimum next target vertex id in the edge sequence
    vertexid_t		edge_mintgt;

    /// index into the edge array, points to the edge object currently edited.
    unsigned int	edge_eidx;

    /// index into the attribute array, points to default bitfield of the
    /// current edge.
    unsigned int	edge_aidx;

    /// attribute array index pointing to next byte to be filled.
    unsigned int	edge_aidxnext;

    /// index of the last attribute id loaded
    unsigned int	edge_lastid;

    /// fill in the remaining attribute values of the current edge
    void                finishEdgeAttrSequence();

public:
    /// create an initialized GraphLoader.
    explicit 	GraphLoader(const class GraphProperties &gp);

    /// finish loading data, returns a reference to the enclosed GraphData. the
    /// reference should be applyGraph()-ed into a GraphContainer.
    class GraphData& finish();

    /// add the termination nodes, this will be called automaticly by finish().
    void 	terminate();

    /** reserve can be used to allocate enough space in the vertex, the edge,
     * and the two attribute arrays. This function need not be called, all
     * arrays auto-grow. Any value may be 0 if it is unknown.
     * @param vertexnum		estimated vertex number of the graph
     * @param edgenum		estimated edges in the graph
     * @param vertexattrsize	size of packed vertex attributes in bytes
     * @param edgeattrsize	size of packed edge attributes in bytes
     */
    void	reserve(unsigned int vertexnum, unsigned int edgenum,
			unsigned int vertexattrsize, unsigned int edgeattrsize);

    /** Creates a new vertex with the given id. It is not strictly necessary to
     * call this function as setVertexAttr will create a non-existing vertex. */
    void	addVertex(vertexid_t vid);

    /** Sets the attribute of a vertex. The vertex id need not exist, however
     * vertex id _and_ attribute id can only be loaded in ascending
     * order, otherwise an OrderException is thrown. */
    void	setVertexAttr(vertexid_t vid, unsigned int attrid, const class AnyType &value);

    /** Creates or starts a new edge from the given src to tgt vertex
     * ids. Again this function is called from setEdgeAttr if the edge doesnt
     * exist. The source vertex must already be loaded via addVertex(). */
    void	addEdge(vertexid_t src, vertexid_t tgt);

    /** Sets the attribute of an edge. The vertex id pair need not exist,
     * however vertex id pair _and_ attribute id can only be loaded in
     * ascending order, otherwise an OrderException is thrown. */
    void	setEdgeAttr(vertexid_t src, vertexid_t tgt,
			    unsigned int attrid, const class AnyType &value);
};

/** \ingroup Exception 
 * Thrown by GraphLoader when the required loading order is violated. */
class OrderException : std::exception
{
};

/** \ingroup Exception 
 * Thrown by GraphLoader if some invalid data is loaded. */
class DataLoadedException : std::exception
{
};

} // namespace VGServer

#endif // VGS_GraphLoader_H
