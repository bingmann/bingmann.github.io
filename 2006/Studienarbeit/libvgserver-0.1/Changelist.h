// $Id: Changelist.h 243 2006-07-06 07:57:25Z bingmann $

#ifndef VGS_ChangeList_H
#define VGS_ChangeList_H

#include "GraphTypes.h"
#include "GraphData.h"
#include "AttributeBlob.h"
#include "GraphPort.h"

#if defined(__GNUC__)
#include <ext/hash_map>
#include <ext/hash_set>
#else
#include <hash_map>
#include <hash_set>
#endif

namespace VGServer {

#if defined(__GNUC__)

using __gnu_cxx::hash_set;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;

#else

using stdext::hash_set;
using stdext::hash_map;
using stdext::hash_multimap;

#endif

/** Changelist contains a set of flexible data structures used to save
 * temporary changes to the graph, and possibly prepare the changes to be
 * incorporated into the global graph. \ingroup Public */

class Changelist
{
private:
    /// takes the change maps directly when applying the Changelist.
    friend class GraphData;

    /// takes the changes directly when merging them into the rtree
    friend class GraphContainer;

    /// reference to the GraphData object for which these represent changes.
    const class GraphData &graph;

    /* The Changelist consists of two hash_maps holding AttributeBlobs which
     * contain the attribute values of the changed vertices and edges. If a
     * vertex or edge is deleted, then the AttributeBlob at that position is
     * empty. Thus addVertex and addEdge allocate the default bitfield and/or
     * the varying attributes. Furthermore the addV/E function add a new entry
     * to the hash_sets of newly create vertices or edges. These are checked
     * when providing frame 0 in getArea. */
    
    typedef hash_map<vertexid_t, AttributeVertexTinyBlob> vertexchglist_t;

    /// hash map used to save changed attribute blobs
    hash_map<vertexid_t, AttributeVertexTinyBlob> vertexchglist;

    typedef hash_set<vertexid_t> vertexaddlist_t;

    /// hash map used to save newly added vertex numbers
    hash_set<vertexid_t>	vertexaddlist;

    /// the edge changes map will be index by a tupel of vertex ids, just like
    /// in math notation
    typedef std::pair<vertexid_t, vertexid_t> vertexidpair_t;

#if defined(__GNUC__)

    /// yes in the hash function and in the equality predicate, only the source
    /// vertex id is used.
    struct vertexidpair_hashfunc {
	inline size_t operator()(const vertexidpair_t &p) const
	{ return p.first; }
    };

    struct vertexidpair_equalfunc {
	inline bool operator()(const vertexidpair_t &a, const vertexidpair_t &b) const
	{ return (a.first == b.first); }
    };

    struct vertexidpair_add_hashfunc { // another hash function for the add list
	inline size_t operator()(const vertexidpair_t &p) const
	{ return (p.first ^ p.second); }
    };

    typedef hash_multimap<vertexidpair_t, AttributeEdgeTinyBlob,
		          vertexidpair_hashfunc, vertexidpair_equalfunc> edgechglist_t;

    typedef hash_set<vertexidpair_t, vertexidpair_add_hashfunc>	edgeaddlist_t;

#else
    // visual c++

    struct vertexidpair_hash_compare {
	static const size_t bucket_size = 4;
	static const size_t min_buckets = 8;

	/// hash function: projection on first vertex.
	inline size_t operator()(const vertexidpair_t &p) const
	{ return p.first; }

	/// equality operator for the pairs.
	inline bool operator()(const vertexidpair_t &a, const vertexidpair_t &b) const
	{ return (a.first == b.first); }
    };

    typedef hash_multimap<vertexidpair_t, AttributeEdgeTinyBlob,
		          vertexidpair_hash_compare> edgechglist_t;

    // TODO: typedef edgeaddlist_t

#endif

    /** For the edge change storage a hash_multimap is used: the direct
     * alternative would be a map<pair,vector<changes> >. But this means that
     * the vector management structures are instanciated for each changed
     * source vertex id. In the hash_multimap more than one source vertex ids
     * are grouped into a bucket. A multimap is needed to supported
     * getEdgeListChange() */

    /// hash map used to save changes to the edges
    edgechglist_t edgechglist;

    /// hash set used to save newly added edges for getArea queries
    edgeaddlist_t edgeaddlist;
    
    /// To find an edge in the edgechangelist, we must only iterate over the
    /// bucket responsible for the src vertex id, because the hash function
    /// groups outgoing edges of a vertex into the same bucket. The STL
    /// hash_multimap::find function returns an iterator to the responsible
    /// bucket, but continuous ++ incrementation of the iterator will not stop
    /// on the bucket boundary. Because the hash_multimap is not sorted, it is
    /// also not possible to stop iterating when the first vertex id is !=
    /// src. The following function is a quick-fix for this problem:
    /// count(pair) will iterate only over one bucket, providing the number of
    /// matching edges so that iteration can later be terminated after the
    /// number of outgoing edges was seen.
    inline edgechglist_t::iterator find_edgechglist(vertexid_t src, vertexid_t tgt)
    {
	unsigned int ec = edgechglist.count(vertexidpair_t(src,tgt));
	if (ec == 0) return edgechglist.end();

	edgechglist_t::iterator e = edgechglist.find(vertexidpair_t(src,tgt));

	while( e != edgechglist.end() && !(e->first.first == src && e->first.second == tgt) )
	{
	    if (e->first.first == src) {
		if (--ec == 0)
		    return edgechglist.end();
	    }
	    e++; // find right target change entry
	}

	return e;
    }

    /// same function as find_edgechglist but returns a const_iterator for a
    /// const object.
    inline edgechglist_t::const_iterator find_edgechglist(vertexid_t src, vertexid_t tgt) const
    {
	unsigned int ec = edgechglist.count(vertexidpair_t(src,tgt));
	if (ec == 0) return edgechglist.end();

	edgechglist_t::const_iterator e = edgechglist.find(vertexidpair_t(src,tgt));

	while( e != edgechglist.end() && !(e->first.first == src && e->first.second == tgt) )
	{
	    if (e->first.first == src) {
		if (--ec == 0)
		    return edgechglist.end();
	    }
	    e++; // find right target change entry
	}

	return e;
    }

public:
    /// constructs a new Changelist for the given graph
    explicit Changelist(const class GraphData &graph);

    /// clear the change list
    void	clear();

    /// adds a new vertex. Returns true for success (the vertex didnt exist yet)
    bool	addVertex(vertexid_t id);

    /// adds a new vertex and fills it with the give blob data. Returns true
    /// for success (the vertex didnt exist yet)
    bool	addVertex(vertexid_t id, const AttributeVertexTinyBlob &vb);

    /// changes the value of an attribute on the vertex
    bool	setVertexAttr(vertexid_t id, unsigned int attrid, const AnyType &value);

    /// change all attribute values on the vertex
    bool	setVertexAttr(vertexid_t id, const AttributeVertexTinyBlob &vb);
    
    /// deletes a vertex from the graph. returns true if it existed
    bool	delVertex(vertexid_t id);
    
    /// adds a new edge from source to target vertex. returns true for success
    bool	addEdge(vertexid_t src, vertexid_t tgt);

    /// adds a new edge from source to target vertex. returns true for success
    bool	addEdge(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &eb);
    
    /// changes the value of an attribute on the edge
    bool	setEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attrid, const AnyType &value);

    /// changes the value of an attribute on the edge
    bool	setEdgeAttr(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &eb);
    
    /// removes an edge from the graph. returns true if it existed.
    bool	delEdge(vertexid_t src, vertexid_t tgt);

    /// check if a vertex was changed
    inline bool	isVertexChanged(vertexid_t id) const
    { return (vertexchglist.find(id) != vertexchglist.end()); }

    /// return the change object for a vertex id (the change object _must_ exist)
    inline const AttributeVertexTinyBlob& getVertexChange(vertexid_t vid) const
    { return vertexchglist.find(vid)->second; }

    /// check if an edge was changed
    inline bool	isEdgeChanged(vertexid_t src, vertexid_t tgt) const
    {
	edgechglist_t::const_iterator e = find_edgechglist(src,tgt);

	return (e != edgechglist.end());
    }

    /// return the change object for an edge (the change object _must_ exist)
    inline const AttributeEdgeTinyBlob& getEdgeChange(vertexid_t src, vertexid_t tgt) const
    {
	edgechglist_t::const_iterator e = find_edgechglist(src,tgt);

	assert(e != edgechglist.end());
	return e->second;
    }

    typedef std::pair<vertexid_t, const AttributeEdgeTinyBlob*> edgeblobpair_t;
    typedef std::vector<edgeblobpair_t> edgelist_t;

    /// returns a sorted vector of changed outgoing edges of a vertex. the vector
    /// contains a pair in order to include the target vertex id.
    std::vector<edgeblobpair_t> getEdgeListChange(vertexid_t src) const;

};

} // namespace VGServer

#endif // VGS_Changelist_H
