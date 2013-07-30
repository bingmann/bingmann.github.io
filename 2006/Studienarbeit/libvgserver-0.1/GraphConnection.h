// $Id: GraphConnection.h 127 2006-05-13 15:30:07Z bingmann $

#ifndef VGS_GraphConnection_H
#define VGS_GraphConnection_H

#include "GraphContainer.h"
#include "ChangeTimeline.h"

namespace VGServer {

/** GraphConnection represents the context and link to exactly one client
 * instance. Each client context has it's local set of changes to the graph
 * made by the application. This class is abstract and must be filled with the
 * application itself, as in most cases the application's interesting issues
 * are limited to a client's context. \ingroup Public */

class GraphConnection
{
private:
    
    class GraphContainer	&graph;

    class ChangeTimeline	changes;

public:
    /// construct a new application scenario connection
    GraphConnection(GraphContainer &graph);

    virtual ~GraphConnection();

    // *** Methods provides to the application:

    // ** transaction methods to clear or commit the changes

    /// abort all changes by clearing the current changelist
    void	rollback();

    /// commit recorded changes into the global graph. warning: slow operation
    void	commit();
    
    // ** local changes operating on the ChangeTimeline and may be committed to
    // ** the global graph

    /// adds a new vertex. Returns true for success (the vertex didnt exist yet)
    inline bool	addVertex(vertexid_t id)
    { return changes.addVertex(id); }

    /// changes the value of an attribute on the vertex
    inline bool	setVertexAttr(vertexid_t id, unsigned int attrid, const AnyType &value)
    { return changes.setVertexAttr(id, attrid, value); }
    
    /// deletes a vertex from the graph. returns true if it existed
    inline bool	delVertex(vertexid_t id)
    { return changes.delVertex(id); }
    
    /// adds a new edge from source to target vertex. returns true for success
    inline bool	addEdge(vertexid_t src, vertexid_t tgt)
    { return changes.addEdge(src, tgt); }
    
    /// changes the value of an attribute on the edge
    inline bool	setEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attrid, const AnyType &value)
    { return changes.setEdgeAttr(src, tgt, attrid, value); }
    
    /// removes an edge from the graph. returns true if it existed.
    inline bool	delEdge(vertexid_t src, vertexid_t tgt)
    { return changes.delEdge(src, tgt); }

    /// advance the change timeline to the next frame. returns the number of
    /// the next frame.
    inline unsigned int advanceTimeFrame()
    { return changes.advanceTimeFrame(); }

    // *** methods to query the local graph snapshot

    /// return a single vertex object
    inline class VertexRef getVertex(vertexid_t id) const
    { return graph.getVertex(id, changes.getChangelistEnd()); }

    /// return a single edge object (if it exists)
    inline class EdgeRef getEdge(vertexid_t src, vertexid_t tgt) const
    { return graph.getEdge(src, tgt, changes.getChangelistEnd()); }

    /// typedef of the return value of getEdgeList
    typedef std::vector<class EdgeRef>	edgereflist_t;

    /// return the list of outgoing edges
    inline edgereflist_t getEdgeList(vertexid_t id) const
    { return graph.getEdgeList(id, changes.getChangelistEnd()); }

    // *** Methods coined for binary transfer to the client

    // *** create facade class for the client
    class GraphClientFace getClientFace();

    /// auxiliary function for the client: return a serialization of the graph
    /// properties including all available attributes.
    void	getGraphProperties(class ByteBuffer &dest) const;

    /// main function for the client: retrieve all vertices and edges within
    /// the specified rectangle which match the expression in filter. Includes
    /// the attributes which's names are in vertexattrsel or edgeattrsel.
    void	getArea(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
			const QueryLimits *ql,
			const char* filter, const char* vertexattrsel, const char* edgeattrsel,
			class ByteBuffer &dest) const;

    /// another function for the client: retrieve within the specified
    /// rectangle the vertex and edge closest to (selx,sely). Returns the
    /// selected attributes.
    void 	getNearestNeighbor(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
				   coord_t selx, coord_t sely, const QueryLimits *ql,
				   const char* filter, const char* vertexattrsel, const char* edgeattrsel,
				   class ByteBuffer &dest) const;
};

} // namespace VGServer

#endif // VGS_GraphConnection_H
