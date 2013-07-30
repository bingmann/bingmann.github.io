// $Id: ChangeTimeline.h 114 2006-04-30 18:01:54Z bingmann $

#ifndef VGS_ChangeTimeline_H
#define VGS_ChangeTimeline_H

#include <deque>
#include <assert.h>

#include "GraphTypes.h"
#include "GraphData.h"
#include "AttributeBlob_fwd.h"

namespace VGServer {

/** ChangeFrame contains a list of changes made to the graph within one time
 * slot. It is currently a simple list, recording the actions in the same order
 * as issued. \ingroup Public */

class ChangeFrame
{
public:
    /// change operation ids used in animation list
    enum changeid_t { CHG_NONE=0,
		      CHG_ADDVERTEX, CHG_SETVERTEX, CHG_DELVERTEX,
		      CHG_ADDEDGE, CHG_SETEDGE, CHG_DELEDGE };

protected:
    /// struct used as entries in the sequence of changes
    struct ChangeEntry
    {
	/// change operation recorded
	changeid_t	chgid;
	
	/// depending on change op: vid1 = vertex id, or (vid1,vid2) edge id.
	unsigned int	vid1, vid2;
	
	/// contains the new blob. this can be either VertexBlob or EdgeBlob.
	AttributeVertexTinyBlob	blob;

	/// initialising constructor for delete changes
	inline ChangeEntry(changeid_t _chgid, unsigned int _vid1, unsigned int _vid2)
	    : chgid(_chgid), vid1(_vid1), vid2(_vid2)
	{
	}

	/// initialising constructor for vertex changes
	inline ChangeEntry(changeid_t _chgid, unsigned int vid, const AttributeVertexTinyBlob &_blob)
	    : chgid(_chgid), vid1(vid), blob(_blob)
	{
	}

	/// initialising constructor for edge changes
	inline ChangeEntry(changeid_t _chgid, unsigned int _vid1, unsigned int _vid2, const AttributeEdgeTinyBlob &eblob)
	    : chgid(_chgid), vid1(_vid1), vid2(_vid2), blob(eblob.data(), eblob.size())
	{
	}
    };

    /// sequence container used to save the change blobs
    typedef std::deque<ChangeEntry>	chgoplist_t;

    chgoplist_t	chgoplist;

    /// direct access to chgoplist in getArea
    friend class GraphContainer;

public:
    /// adds a new vertex id, without setting attributes.
    void	addVertex(vertexid_t id, const AttributeVertexTinyBlob &b);

    /// sets an attribute on the given vertex
    void	setVertexAttr(vertexid_t id, const AttributeVertexTinyBlob &b);

    /// deletes a vertex from the graph.
    void	delVertex(vertexid_t id);
    
    /// adds a new edge from source to target vertex.
    void	addEdge(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &b);
    
    /// sets an attribute on the given edge
    void	setEdgeAttr(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &b);

    /// removes an edge from the graph.
    void	delEdge(vertexid_t src, vertexid_t tgt);
};

/** ChangeTimeline models the functions to manage two Changelist snapshots: one
 * of the initial frame and one including all ChangeFrames merged to support
 * queries for the final state. Furthermore it records the changes made in the
 * animation into time slots represented by a list of ChangeFrames. */

class ChangeTimeline
{
protected:
    // Implementation: for a trivial timeline containing only one time slot,
    // clstart == clend is the only changelist, and the ChangeFrame list is
    // empty. Once the first time slot is added, the clend becomes a copy of
    // clstart, and is augmented with all further changes, while clstart
    // remains unchanged.

    /// the changes made before the first frame is marked as finished. this
    /// structure is used by the query functions of the client, as this is the
    /// base for it's animation list.
    class Changelist	*clstart;

    /// represents the changes made during the whole timeline, thus is a merged
    /// version from clstart with all ChangeFrames.
    class Changelist	*clend;

    /// reference to the GraphData object for which these represent changes.
    const class GraphData &graph;

    /// list of frames. each frame records which changes happened during it's
    /// time. Except for the first time slot, it's saved directly in the clstart.
    typedef std::deque<class ChangeFrame> changeframelist_t;

    /// the actual list of frames. if this is empty, the timeline consist of
    /// only one changelist.
    changeframelist_t	frames;

    /// accesses the frames in getArea
    friend class GraphContainer;

public:
    /// constructs a new ChangeTimeline for the given graph
    ChangeTimeline(const class GraphData &graph);

    ~ChangeTimeline();

    /// clear the change list
    void	clear();

    /// adds a new vertex. Returns true for success (the vertex didnt exist yet)
    bool	addVertex(vertexid_t id);

    /// changes the value of an attribute on the vertex
    bool	setVertexAttr(vertexid_t id, unsigned int attrid, const AnyType &value);
    
    /// deletes a vertex from the graph. returns true if it existed
    bool	delVertex(vertexid_t id);
    
    /// adds a new edge from source to target vertex. returns true for success
    bool	addEdge(vertexid_t src, vertexid_t tgt);
    
    /// changes the value of an attribute on the edge
    bool	setEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attrid, const AnyType &value);
    
    /// removes an edge from the graph. returns true if it existed.
    bool	delEdge(vertexid_t src, vertexid_t tgt);

    /// advance the change timeline to the next frame. returns the number of
    /// the next frame.
    unsigned int advanceTimeFrame();

    /// the number of animation frames in the list, and also the number of the
    /// current frame, because 0 is not stored as a frame.
    inline unsigned int	getFrameCount() const
    { return static_cast<unsigned int>(frames.size()); }

    // *** Functions to access the two changelists. Used by GraphConnection in
    // *** the right places.

    /// returns the changelist of the initial frame (before the first nextFrame() call)
    inline const class Changelist &getChangelistStart() const
    { return *clstart; }

    /// returns the full changelist of the last frame
    inline const class Changelist &getChangelistEnd() const
    { return *clend; }

    /// checks that just one frame exists and returns the changelist
    inline const class Changelist &getChangelist() const
    { assert(clstart == clend); return *clstart; }

};

} // namespace VGServer

#endif // VGS_Changelist_H
