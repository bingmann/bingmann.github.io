// $Id: GraphData.h 241 2006-07-05 07:29:52Z bingmann $

#ifndef VGS_GraphData_H
#define VGS_GraphData_H

#include <vector>
#include <ostream>

#include "GraphTypes.h"
#include "GraphProperties.h"
#include "AttributeBlob.h"
#include "TpArray.h"

namespace VGServer {

/** GraphData contains the basic data structures representing the global static
 * graph. It is represented by the vertex and it's adjacency arrays. This data
 * could also be put into GraphContainer directly, but by separating it, a new
 * graph can be prepared (e.g. loaded from file by GraphLoader) and activated
 * while the server is still handling requests. \ingroup Public */

class GraphData
{
public:

    /// allocated millions of times in the vertex array
    struct Vertex
    {
	/// index into the edge adjaceny array of the first edge
	unsigned int	edgeidx;

	/// index into the attribute value array of the default bitfield (if it
	/// exists)
	unsigned int	attridx;

	/// returns the value of a vertex attribute by fetching the data from
	/// the GraphData object's arrays
	inline class AnyType getAttr(unsigned int attrid, const GraphData &gd) const
	{ return gd.vertexattr.getAttrChainValue(attridx, attrid, gd.properties.vertexattr); }
    };

    /// allocated millions of times in the adjacency array
    struct Edge
    {
	/// target vertex id. if the graph is undirected then the array only
	/// contains vertex ids smaller than the owner vertex id.
        vertexid_t	target;

	/// index into the attribute value array of the default bitfield (if it
	/// exists)
	unsigned int	attridx;

	/// returns the value of an edge attribute by fetching the data from
	/// the GraphData object's arrays
	inline class AnyType getAttr(unsigned int attrid, const GraphData &gd) const
	{ return gd.edgeattr.getAttrChainValue(attridx, attrid, gd.properties.edgeattr); }
    };

public://private:
    /// initialized by the constructor
    class GraphProperties	properties;

    /// STL vector is used with caution: it should never auto-grow.
    TpArray<Vertex>	vertices;

    /// STL vector containing the adjacency array
    TpArray<Edge>	edges;

    /// byte blob containing all vertex attributes packed using default
    /// bitfields and fixed lengths specified in the GraphProperties.
    AttributeBigBlob	vertexattr;

    /// byte blob containing all edge attributes packed
    AttributeBigBlob	edgeattr;

    /// cached number of valid (non-blank) vertices, this is not
    /// vertices.size() because the vertex array may contain blank objects
    /// which are viewed as non-existing.
    unsigned int	vertexcount;

    /// accesses vertices and edges directly read-only
    friend class GraphContainer;
    
    /// directly writes the data into the arrays
    friend class GraphLoader;

    /// accesses protected functions through GraphContainer
    friend class Changelist;

    /// direct access into the vertex array
    friend class VertexRef;

    /// direct access into the edge array
    friend class EdgeRef;

public:
    // *** Functions are all public to the application, but GraphContainer
    // *** provides an additional layer with the Changelist.

    explicit GraphData(const class GraphProperties &properties);

    /// this will swap the data holder objects with the other GraphData object.
    void	swap(class GraphData &other);

    /// this takes the changes from cl and merges them into the current graph
    /// data. this operation takes a long time, because the arrays are
    /// reconstructed. care should be taken not to access the data in this
    /// time.
    void	applyChangelist(const class Changelist &cl);

    /// returns the currently used GraphProperties object
    inline const GraphProperties& getProperties() const
    { return properties; }

    // *** Vertex Array Functions

    /// return a single vertex object
    const Vertex* getVertex(vertexid_t id) const;

    /// checks if the vertex id exists, that is if it has any attribute data
    bool	existVertex(vertexid_t id) const;

    /// returns the value of a vertex attribute by id
    class AnyType getVertexAttr(vertexid_t id, unsigned int attr) const;

    /// returns a copy of the attribute values of a vertex as a TinyBlob.
    AttributeVertexTinyBlob getVertexAttrBlob(vertexid_t id) const;

    /// returns the number of non-blank vertices
    unsigned int getVertexCount() const
    { return vertexcount; }

    // *** Edge Array Functions

    /// return a single edge object (if it exists)
    const Edge* getEdge(vertexid_t src, vertexid_t tgt) const;

    /// returns the index of a valid edge into the edges array
    unsigned int getEdgeIdx(vertexid_t src, vertexid_t tgt) const;

    /// checks if an edge exists
    bool	existEdge(vertexid_t src, vertexid_t tgt) const;

    /// returns the value of an edge attribute by (src,tgt) vertex ids
    class AnyType getEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attr) const;

    /// returns a copy of the attribute values of an edge as a TinyBlob.
    AttributeEdgeTinyBlob getEdgeAttrBlob(vertexid_t src, vertexid_t tgt) const;

    /// return the list of outgoing edges
    std::vector<const Edge*> getEdgeList(vertexid_t id) const;

    /// return the list of outgoing edge indices
    std::vector<unsigned int> getEdgeIdxList(vertexid_t id) const;

    /// return a list of destination indices of the outgoing edges of srcid
    std::vector<unsigned int> getEdgeTargetList(vertexid_t srcid) const;

    /// returns the number of valid edges
    unsigned int getEdgeCount() const
    { return edges.size()-1; }

    /// returns number of used bytes in the vertex attribute array
    unsigned int getVertexAttrBytes() const
    { return vertices.last().attridx; }

    /// returns number of used bytes in the edge attribute array
    unsigned int getEdgeAttrBytes() const
    { return edges.last().attridx; }

public:
    // *** Debugging Functions

    /// write the global static graph arrays as a snapshot which can be loaded
    /// very fast.
    void	saveSnapshot(std::ostream &s) const;

    /// reload the global static graph arrays from a snapshot
    bool	loadSnapshot(std::istream &s);

    /// write the whole graph as a fig file. this can get very big
    void	writeFig(bool writefigheader, std::ostream &s, unsigned int attrX, unsigned int attrY) const;

    /// checks all adjacency array references
    bool	checkReferences() const;
};

} // namespace VGServer

#endif // VGS_GraphData_H
