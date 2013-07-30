// $Id: GraphContainer.h 231 2006-06-25 20:00:02Z bingmann $

#ifndef VGS_GraphContainer_H
#define VGS_GraphContainer_H

#include "GraphTypes.h"
#include "GraphData.h"
#include "ByteBuffer.h"
#include "RTree.h"

#include <map>

#if defined(__GNUC__)
#include <ext/hash_set>
#else
#include <hash_set>
#endif

namespace VGServer {

#if defined(__GNUC__)
using __gnu_cxx::hash_set;
#else
using stdext::hash_set;
#endif

/** GraphContainer is the owner class of the global static graph
 * information. This makes it the place in which the threads of the client
 * connections have to be synchronized. It will also manage additional data
 * structures used for efficient searching in the graph. \ingroup Public */

class GraphContainer : public GraphData
{
public:
    //typedef class RTree<coord_t, long long> RTree_t;
    typedef class RTree RTree_t;

    /// structure which contains the data for each rectangle in the leaves of
    /// the rtree
    struct RTreeData
    {
	// vertex id's
	unsigned int src, tgt;

	RTreeData()
	{ }

	RTreeData(unsigned int _src, unsigned int _tgt)
	    : src(_src), tgt(_tgt)
	{ }

	inline bool operator==(const RTreeData &o) const
	{ return (src == o.src) && (tgt == o.tgt); }
    };

    /// callback context class used to get a MBR from a data object
    struct RTreeDataCallback
    {
	GraphContainer &gc;

	RTreeDataCallback(GraphContainer &_gc)
	    : gc(_gc)
	{ }
	
	inline const class RTree_t::Rect getMBR(const RTreeData &d) const
	{
	    int x1 = gc.getVertexAttr(d.src, gc.vertexattrid_xaxis).getInteger();
	    int y1 = gc.getVertexAttr(d.src, gc.vertexattrid_yaxis).getInteger();

	    int x2 = gc.getVertexAttr(d.tgt, gc.vertexattrid_xaxis).getInteger();
	    int y2 = gc.getVertexAttr(d.tgt, gc.vertexattrid_yaxis).getInteger();

	    return RTree_t::Rect(std::min(x1,x2), std::min(y1,y2),
				 std::max(x1,x2), std::max(y1,y2));
	}
    };

    typedef RTree_t::Tree<struct RTreeData, struct RTreeDataCallback> GraphRTree;

private:
    static const bool rtreemap_descending;

    /// struct used as Comparison Relation for the rtreemap
    struct rtreemap_compare
    {
	bool descending;

	rtreemap_compare(bool _desc) : descending(_desc) { }

	inline bool operator()(unsigned int a, unsigned int b) const {
	    return descending ? (a > b) : (a < b);
	}
    };

    /// multi-level std::map binary red-black tree
    typedef std::map<unsigned int, GraphRTree, rtreemap_compare> rtreemap_t;
    rtreemap_t		rtreemap;

    /// clear the rtree and reinsert all edges
    void	recreateRTree();

    /// callback object to extract coordinates on the fly
    RTreeDataCallback	rtree_callback;

    /// Axis used to build the R-Tree Levels
    unsigned int vertexattrid_xaxis, vertexattrid_yaxis;

    unsigned int edgeattrid_zaxis;

    /// number of edges returned in the last getArea query. used for
    /// experiments
    mutable unsigned int lastqueryedgenum;

public:
    // *** Instead of returning pointers into the vertex or edge arrays, the
    // *** functions of GraphContainer will return objects holding references
    // *** to the various places allowing them to return further information.
    // *** These Ref objects (VertexRef and EdgeRef) are declared below.

    unsigned int reinsertnum;
public:
    explicit GraphContainer(const class GraphProperties &properties);

    ~GraphContainer();

    /// atomicly changes the graph data, swaps the old data into newgraph and
    /// updates index structures
    void	applyGraphData(class GraphData &newgraph);

    /// atomicly swaps the graph data and index structures from the other
    /// object.
    void	swap(class GraphContainer &other);

    //*** essentially just pass-thru functions

    /// this takes the changes from cl and merges them into the current graph
    /// data. this operation takes a long time, because the arrays are
    /// reconstructed. care should be taken not to access the data in this
    /// time.
    void	applyChangelist(const class Changelist &cl);

    /// this takes the changes from cl and merges them into the current graph
    /// data. this operation takes a long time, because the arrays are
    /// reconstructed. care should be taken not to access the data in this
    /// time.
    void	applyChangeTimeline(const class ChangeTimeline &ct);

    /// write the global static graph arrays as a snapshot which can be loaded
    /// very fast. this includes the r-tree
    void	saveSnapshot(std::ostream &s) const;

    /// reload the global static graph arrays from a snapshot including the
    /// r-tree
    bool	loadSnapshot(std::istream &s);

    //*** these functions unify the information in the global graph and that in
    //*** the parameter Changelists

    /// return a single vertex reference object
    class VertexRef getVertex(vertexid_t id, const class Changelist &cl) const;

    /// return a single edge reference object
    class EdgeRef getEdge(vertexid_t src, vertexid_t tgt, const class Changelist &cl) const;

    /// return the list of outgoing edge references
    std::vector<class EdgeRef> getEdgeList(vertexid_t id, const class Changelist &cl) const;

    //*** provided by search data structures
    //*** (or for the beginning by linear algorithms)

    void getGraphProperties(class ByteBuffer &dest) const;
    
    /// main function for the client: retrieve all vertices and edges within
    /// the specified rectangle which match the expression in filter. Includes
    /// the attributes which's names are in vertexattrsel or edgeattrsel.
    void 	getArea(coord_t x1, coord_t y1, coord_t x2, coord_t y2, const QueryLimits *ql,
			const char* filter, const char* vertexattrsel, const char* edgeattrsel,
			const class ChangeTimeline &ct, class ByteBuffer &dest) const;

    /// auxiliary function for the client to handle a mouse click on the world
    void	getNearestNeighbor(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
				   coord_t selx, coord_t sely, const QueryLimits *ql,
				   const char* filter, const char* vertexattrsel, const char* edgeattrsel,
				   const class Changelist &cl, class ByteBuffer &dest) const;

    /// function for experiments: return the number of edges returned in the
    /// last query
    inline unsigned int getLastQueryEdgenum() const
    { return lastqueryedgenum; }

    /// return number of rtrees
    inline unsigned int getRTreeNum() const
    { return rtreemap.size(); }

    typedef RTree_t::Stats RTreeStats_t;
    typedef RTree_t::LevelStats RTreeLevelStats_t;

    /// return the statistics struct caluclated from all rtrees
    void getRTreeStats(RTreeStats_t &st) const;

private:
    /// class used as a visitor when walking the R-Tree
    class Visitor
    {
    private:
	// *** References to parent objects
	const class GraphContainer &gc;
	const class AttributeSelectorList &asl_vertex, &asl_edge;
	const class FilterRoot &filter;
	const class Changelist &cl;

	/// temp attr blob to concat
	AttributeVertexTinyBlob attrblob;
	
        /// maxlimits set by parent
	unsigned int vertexmaxlimit, edgemaxlimit;

	/// saved byte of filter
	int	filtertype;

	/// hash_set to remove duplicate vertex info from result set
	hash_set<unsigned int>	&vertexdup;

    public:
	/// output buffer
	ByteBuffer vertexout, edgeout;

	/// number of vertices and edges selected into the output buffer
	unsigned int vertexnum, edgenum;

	Visitor(const GraphContainer &_gc,
		const class AttributeSelectorList &_asl_vertex, const class AttributeSelectorList &_asl_edge,
		const class FilterRoot &_filter,
		const class Changelist &_cl, unsigned int _vertexmaxlimit, unsigned int _edgemaxlimit,
		hash_set<unsigned int> &vertexdup);

	/// called by RTree::queryRange() 
	bool operator()(const RTree_t::Rect &, const RTreeData &d);

	/// used to add the selected attributes of a vertex to the vertexout
	/// buffer
	void	add_vertex(unsigned int vid);

	/// used to add the selected attributes of a edge to the edgeout
	/// buffer
	void	add_edge(unsigned int src, unsigned int tgt);
    };

    /// class used to save the current nearest neighbor result
    class NNResult
    {
    public:
	/// the query point
	class RTree_t::Point point;

	/// nearest vertex id found
	unsigned int	nn_vid;

	/// nearest edge found, described as src,tgt pair
	unsigned int	nn_esrc, nn_etgt;

	/// square distance to the vertex coordinates
	RTree_t::AreaType	dist_nn_vid;

	/// square distance to the edge
	RTree_t::AreaType 	dist_nn_edge;

	/// initializer for the first NNResult object
	inline NNResult(const RTree_t::Point &p);

	/// update the min fields for this vertex's info
	inline void	checkVertex(coord_t x, coord_t y, unsigned int vid);

	/// update the min fields for this edge's info
	inline void	checkEdge(coord_t x1, coord_t y1, coord_t x2, coord_t y2, unsigned int vsrc, unsigned int vtgt);
    };

    /// class used as a visitor when walking the R-Tree for nearest neighbor
    class NNVisitor
    {
    private:
	// *** References to parent objects
	const class GraphContainer &gc;
	const bool use_asl_vertex, use_asl_edge;
	const class FilterRoot &filter;
	const class Changelist &cl;

        /// maxlimits set by parent
	unsigned int vertexmaxlimit, edgemaxlimit;

	/// saved byte of filter
	int	filtertype;

	/// hash_set to remove duplicate vertex info from result set
	hash_set<unsigned int>	&vertexdup;

    public:
	/// number of vertices and edges selected into the output buffer
	unsigned int vertexnum, edgenum;

	/// contain a copy of the current neighbor query context
	class NNResult nnr;

	NNVisitor(const GraphContainer &_gc, const class NNResult &nnr,
		  const class AttributeSelectorList &_asl_vertex, const class AttributeSelectorList &_asl_edge,
		  const class FilterRoot &_filter,
		  const class Changelist &_cl, unsigned int _vertexmaxlimit, unsigned int _edgemaxlimit,
		  hash_set<unsigned int> &vertexdup);

	/// called by RTree::queryRange() 
	bool operator()(const RTree_t::Rect &, const RTreeData &d);

	/// significantly reduced function: just increment vertexnum
	void	add_vertex(unsigned int vid);

	/// significantly reduced function: just increment edgenum
	void	add_edge(unsigned int src, unsigned int tgt);
    };
};

/// VertexRef is a read-only reference object holding indices into the
/// GraphData structures. It provides methods to extract attribute data. The
/// references are valid as long as the queried GraphData object is valid.
class VertexRef
{
private:
    /// reference to the GraphData object, used for properties and data
    const class GraphData* graphdata;

    /// vertex id of the referenced vertex
    unsigned int vertexid;

    /// if the attribute chain was modified then this is a copy from the
    /// Changelist.
    const AttributeVertexTinyBlob* attrmodified;

protected:
    /// protected constructor for GraphContaineer::getVertex()
    inline VertexRef(const class GraphData* gd, unsigned int vid)
	: graphdata(gd), vertexid(vid), attrmodified(NULL)
    {
    }

    /// protected constructor for GraphContaineer::getVertex() with modified attribute chain
    inline VertexRef(const class GraphData* gd, unsigned int vid, const AttributeVertexTinyBlob& am)
	: graphdata(gd), vertexid(vid), attrmodified(new AttributeVertexTinyBlob(am))
    {
    }

    /// access to the protected constructors
    friend class GraphContainer;

public:
    /// copy constructor
    inline VertexRef(const VertexRef &orig)
	: graphdata(orig.graphdata), vertexid(orig.vertexid)
    {
	attrmodified = orig.attrmodified ? new AttributeVertexTinyBlob(*orig.attrmodified) : NULL;
    }

    /// assignment operator
    inline VertexRef& operator=(const VertexRef &orig)
    {
	if (attrmodified) delete attrmodified;
	graphdata = orig.graphdata;
	vertexid = orig.vertexid;
	attrmodified = orig.attrmodified ? new AttributeVertexTinyBlob(*orig.attrmodified) : NULL;
	return *this;
    }

    /// destructor conditionally deletes the copied attribute blob chain
    inline ~VertexRef()
    {
	if (attrmodified) delete attrmodified;
    }

    /// checks that the vertex is valid
    inline bool valid() const
    { return graphdata != NULL; }

    /// returns the vertex id
    inline vertexid_t getId() const
    { return vertexid; }

    /// returns the value of an attribute
    inline AnyType getAttr(unsigned int attrid) const
    {
	assert(graphdata);	// programs should call valid() first

	if (attrmodified)
	    return attrmodified->getAttrChainValue(0, attrid, graphdata->getProperties().vertexattr);

	return graphdata->vertices[vertexid].getAttr(attrid, *graphdata);
    }
};

/// EdgeRef is a read-only reference object holding indices into the
/// GraphData structures. It provides methods to extract attribute data. The
/// references are valid as long as the queried GraphData object is valid.
class EdgeRef
{
private:
    /// reference to the GraphData object, used for properties and data
    const class GraphData* graphdata;

    /// vertex id of the source vertex
    vertexid_t srcvid;

    /// pointer into the edge array to the referenced edge object. if the edge
    /// was modified using the change list, then this unsigned int is misused
    /// as the target vertex id;
    unsigned int edgeidx;

    /// if the attribute chain was modified then this is a copy from the
    /// Changelist.
    const AttributeEdgeTinyBlob* attrmodified;

protected:
    /// protected constructor for GraphContaineer::getEdge()
    inline EdgeRef(const class GraphData* gd, unsigned int _srcvid, unsigned int _edgeidx)
	: graphdata(gd), srcvid(_srcvid), edgeidx(_edgeidx), attrmodified(NULL)
    {
    }

    /// protected constructor for GraphContaineer::getEdge() with modified attribute chain
    inline EdgeRef(const class GraphData* gd, unsigned int _srcvid, unsigned int _edgeidx, const AttributeEdgeTinyBlob& am)
	: graphdata(gd), srcvid(_srcvid), edgeidx(_edgeidx), attrmodified(new AttributeEdgeTinyBlob(am))
    {
    }

    /// access to the protected constructors
    friend class GraphContainer;

public:
    /// copy constructor
    inline EdgeRef(const EdgeRef &orig)
	: graphdata(orig.graphdata), srcvid(orig.srcvid), edgeidx(orig.edgeidx)
    {
	attrmodified = orig.attrmodified ? new AttributeEdgeTinyBlob(*orig.attrmodified) : NULL;
    }

    /// assignment operator
    inline EdgeRef& operator=(const EdgeRef &orig)
    {
	if (attrmodified) delete attrmodified;
	graphdata = orig.graphdata;
	srcvid = orig.srcvid;
	edgeidx = orig.edgeidx;
	attrmodified = orig.attrmodified ? new AttributeEdgeTinyBlob(*orig.attrmodified) : NULL;
	return *this;
    }

    /// destructor conditionally deletes the copied attribute blob chain
    inline ~EdgeRef()
    {
	if (attrmodified) delete attrmodified;
    }

    /// checks that the vertex is valid
    inline bool valid() const
    { return graphdata != NULL; }

    /// returns the source vertex id
    inline vertexid_t getSource() const
    { return srcvid; }

    /// returns the target vertex id
    inline vertexid_t getTarget() const
    {
	if (attrmodified) return edgeidx;
	assert(graphdata);
	return graphdata->edges[edgeidx].target;
    }

    /// returns the value of an attribute
    inline AnyType getAttr(unsigned int attrid) const
    {
	assert(graphdata);	// programs should call valid() first

	if (attrmodified)
	    return attrmodified->getAttrChainValue(0, attrid, graphdata->getProperties().edgeattr);

	return graphdata->edges[edgeidx].getAttr(attrid, *graphdata);
    }

};

} // namespace VGServer

#endif // VGS_GraphContainer_H
