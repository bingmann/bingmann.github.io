// $Id: GraphContainer.cc 243 2006-07-06 07:57:25Z bingmann $

#include "GraphContainer.h"
#include "GraphData.h"
#include "Changelist.h"
#include "ChangeTimeline.h"
#include "ByteOutBuffer.h"
#include "AttributeParser.h"

#include "AttributeBlob_impl.h"
#include "RTree.h"

#include <iostream>
#include <deque>
#include <sys/time.h>

//#define DBGOUTPUT

// disable this for testing rtree speed
#define CALCATTRVALUES

namespace VGServer {

const bool GraphContainer::rtreemap_descending = false;

// force complete instanciation of the rtree
template class RTree::Tree<struct GraphContainer::RTreeData, struct GraphContainer::RTreeDataCallback>;

inline double timestamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return double(tp.tv_sec) + tp.tv_usec / 1000000.;
}

GraphContainer::GraphContainer(const class GraphProperties &properties)
    : GraphData(properties), rtreemap(rtreemap_compare(rtreemap_descending)),
      rtree_callback(*this)
{
    vertexattrid_xaxis = 0;
    vertexattrid_yaxis = 1;
    edgeattrid_zaxis = 2;

    reinsertnum = 9;
}

GraphContainer::~GraphContainer()
{
}

void GraphContainer::applyGraphData(class GraphData &newgraph)
{
    // TODO: thread synchronization here
    GraphData::swap(newgraph);

    // reconstruct new index stuff
    recreateRTree();
}

void GraphContainer::swap(class GraphContainer &newgraph)
{
    // TODO: thread synchronization here

    // swap graph data arrays
    GraphData::swap(newgraph);

    // swap index structures
    std::swap(rtreemap, newgraph.rtreemap);
    
    // update callback pointers, because std::map swap doesnt use assignment
    for(rtreemap_t::iterator ti = rtreemap.begin(); ti != rtreemap.end(); ++ti)
    {
	ti->second.setCallback(&rtree_callback);
    }
    
    std::swap(vertexattrid_xaxis, newgraph.vertexattrid_xaxis);
    std::swap(vertexattrid_yaxis, newgraph.vertexattrid_yaxis);

    std::swap(edgeattrid_zaxis, newgraph.edgeattrid_zaxis);
}

void GraphContainer::recreateRTree()
{
    std::cerr << "Rebuilding R-Tree: 0%";
    std::cerr.flush();

    double ts1 = timestamp();

    rtreemap.clear();

    unsigned int dotstepmax = (edges.size()-1) / 60;
    unsigned int dotstep = 0;
    unsigned int percentstepmax = 6;
    unsigned int percentstep = 0;

    for(vertexid_t vi = 0; vi < vertices.size()-1; vi++)
    {
	if (not existVertex(vi)) continue;

	//int x1 = getVertexAttr(vi, vertexattrid_xaxis).getInteger();
	//int y1 = getVertexAttr(vi, vertexattrid_yaxis).getInteger();

	unsigned int endedgeidx = vertices[vi+1].edgeidx;

	for(unsigned int ei = vertices[vi].edgeidx; ei < endedgeidx; ei++)
	{
	    vertexid_t tvi = edges[ei].target;
	    if (tvi == VERTEX_INVALID) break;
	    if (not existVertex(tvi)) continue;

	    //int x2 = getVertexAttr(tvi, vertexattrid_xaxis).getInteger();
	    //int y2 = getVertexAttr(tvi, vertexattrid_yaxis).getInteger();

	    unsigned int level = edges[ei].getAttr(edgeattrid_zaxis, *this).getInteger();

	    rtreemap_t::iterator rtreei = rtreemap.find(level);

	    if (rtreei == rtreemap.end())
	    {
		// create new R-Tree instance
		rtreei = rtreemap.insert( rtreemap_t::value_type(level, GraphRTree(&rtree_callback)) ).first;
		rtreei->second.setReinsertNum(reinsertnum);
	    }
	    
	    // the rectangle itself is computed on-demand by the RTreeDataCallback
	    rtreei->second.insertRect(RTreeData(vi, tvi));

	    // dumb and funny progress status
	    if (++dotstep >= dotstepmax) {
		std::cerr << ".";
		dotstep = 0;

		if (++percentstep > percentstepmax)
		{
		    std::cerr << int(double(ei) / double(edges.size()-1) * 100.0) << "%";
		    percentstep = 0;
		}

		std::cerr.flush();
	    }
	}
    }
    
    std::cerr << "100% done\n";
    double ts2 = timestamp();

    std::cerr << "Time: " << (ts2-ts1) << "\n";

    // calculate statistics
    RTree_t::Stats st;

    getRTreeStats(st);

    RTree_t::LevelStats lst;

    std::cerr << "RTree Stats: levels = " << st.level.size() << "\n";

    for(int li = st.level.size()-1; li >= 0; li--)
    {
	std::cerr << "l " << li << " " << st.level[li] << "\n";
	lst += st.level[li];
    }
    
    std::cerr << "total: " << lst << "\n";
}

void GraphContainer::getRTreeStats(RTreeStats_t &st) const
{
    for(rtreemap_t::const_iterator ti = rtreemap.begin(); ti != rtreemap.end(); ++ti)
    {
	ti->second.calcStats(st);
    }
}

void GraphContainer::applyChangelist(const class Changelist &cl)
{
    // first delete all modified or deleted changes from the rtrees
    {
	Changelist::edgechglist_t::const_iterator e;
	for(e = cl.edgechglist.begin(); e != cl.edgechglist.end(); e++)
	{
	    if (e->second.empty() || // modification was a delete
		(cl.edgeaddlist.find( e->first ) == cl.edgeaddlist.end()) // not new vertex
		)
	    {
		// retrieve the level on which to find this edge
		unsigned int level = GraphData::getEdgeAttr(e->first.first, e->first.second,
							    edgeattrid_zaxis).getInteger();

		rtreemap_t::iterator rtreei = rtreemap.find(level);

		if (rtreei == rtreemap.end()) {
		    std::cerr << "applyChangelist: could not find rtree level to delete from\n";
		}
		else if (!rtreei->second.deleteRect( RTreeData(e->first.first, e->first.second) )) {
		    std::cerr << "Could not find rect in rtree\n";
		}
	    }
	}
    }

    GraphData::applyChangelist(cl);

    // afterwards add all new edges and reinsert the modified ones
    {
	Changelist::edgechglist_t::const_iterator e;
	for(e = cl.edgechglist.begin(); e != cl.edgechglist.end(); e++)
	{
	    if (!e->second.empty()) // modification was a delete
	    {
		// retrieve the level on which to find this edge
		unsigned int level = GraphData::getEdgeAttr(e->first.first, e->first.second,
							    edgeattrid_zaxis).getInteger();

		rtreemap_t::iterator rtreei = rtreemap.find(level);

		if (rtreei == rtreemap.end()) {
		    // create new R-Tree instance
		    rtreei = rtreemap.insert( rtreemap_t::value_type(level, GraphRTree(&rtree_callback)) ).first;
		    rtreei->second.setReinsertNum(reinsertnum);
		}

		rtreei->second.insertRect( RTreeData(e->first.first, e->first.second) );
	    }
	}
    }

    // check the number of rectangles
#ifndef NDEBUG
    {
	unsigned int rectcount = 0;

	for(rtreemap_t::const_iterator ti = rtreemap.begin(); ti != rtreemap.end(); ++ti)
	{
	    rectcount += ti->second.size();
	}
	assert(rectcount == getEdgeCount());
    }
#endif
}

void GraphContainer::applyChangeTimeline(const class ChangeTimeline &ct)
{
    applyChangelist(ct.getChangelistEnd());
}

void GraphContainer::saveSnapshot(std::ostream &s) const
{
    // first write a signature
    unsigned int key = 0x23232323;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    // *** call the function to save the GraphData
    key = 0x00000001;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    GraphData::saveSnapshot(s);

    // *** then save the r-trees
    key = 0x00000002;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    // save size of map
    key = static_cast<unsigned int>(rtreemap.size());
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    for(rtreemap_t::const_iterator ti = rtreemap.begin(); ti != rtreemap.end(); ++ti)
    {
	// save map key
	key = ti->first;
	s.write(reinterpret_cast<char*>(&key), sizeof(key));

	// write the r-tree serialization
	ti->second.saveSnapshot(s);
    }

    // finished
    key = 0x00000000;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
}

bool GraphContainer::loadSnapshot(std::istream &s)
{
    unsigned int key;
    
    // read signature
    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
    if (key != 0x23232323) return false;

    // read vertices
    while( s.read(reinterpret_cast<char*>(&key), sizeof(key)) )
    {
	switch(key)
	{
	case 0: // termination
	    return true;

	case 1: // graph data: call upwards.
	{
	    if (!GraphData::loadSnapshot(s)) return false;
	    break;
	}
	case 2: // r-trees
	{
	    unsigned int len;
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    std::cerr << "Loading " << len << " R-Trees." << std::endl;

	    rtreemap.clear();
	    
	    for(unsigned int l = 0; l < len; l++)
	    {
		if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
		
		rtreemap_t::iterator ti = rtreemap.find(key);

		if (ti != rtreemap.end())
		    return false;

		// create new R-Tree instance
		ti = rtreemap.insert( rtreemap_t::value_type(key, GraphRTree(&rtree_callback)) ).first;

		if (!ti->second.loadSnapshot(s)) return false;
	    }

	    break;
	}
	default:
	    return false;
	}
    }

    // while loop terminated by istream error
    return false;
}

class VertexRef GraphContainer::getVertex(vertexid_t vid, const class Changelist &cl) const
{
    if (cl.isVertexChanged(vid))
    {
	const AttributeVertexTinyBlob& vc = cl.getVertexChange(vid);

	if (vc.empty()) { // the modification was a delVertex, so return an invalid VertexRef
	    return VertexRef(NULL, 0);
	}

	// construct the reference including a copy of the modifications
	return VertexRef(this, vid, vc);
    }

    if (not GraphData::existVertex(vid))
	    return VertexRef(NULL, 0);

    return VertexRef(this, vid);
}

class EdgeRef GraphContainer::getEdge(vertexid_t src, vertexid_t tgt, const class Changelist &cl) const
{
    if (cl.isEdgeChanged(src, tgt))
    {
	const AttributeEdgeTinyBlob &ec = cl.getEdgeChange(src, tgt);

	if (ec.empty())
	    return EdgeRef(NULL, 0, 0);	// return a blank EdgeRef

	return EdgeRef(this, src, tgt, ec);
    }

    unsigned int eidx = getEdgeIdx(src, tgt);

    if (eidx >= edges.size()-1)
	return EdgeRef(NULL, 0, 0);

    return EdgeRef(this, src, eidx);
}

std::vector<class EdgeRef> GraphContainer::getEdgeList(vertexid_t vid, const class Changelist &cl) const
{
    std::vector<class EdgeRef> edgelist;

    // list of edge in global graph
    int startedgeidx, endedgeidx;

    if (vid >= vertices.size()-1) // source is outside the global graph's range
    {
	startedgeidx = endedgeidx = 0;
    }
    else
    {
	startedgeidx = vertices[vid].edgeidx;
	endedgeidx = vertices[vid+1].edgeidx;
    }

    // merge in the Changelist
    Changelist::edgelist_t ecl = cl.getEdgeListChange(vid);
    
    std::vector<unsigned int>::iterator eii;
    Changelist::edgelist_t::iterator eci = ecl.begin();

    for(int edgeidx = startedgeidx; edgeidx < endedgeidx; )
    {
	// add new edges that come before the eii index.
	while (eci != ecl.end() and eci->first < edges[edgeidx].target)
	{
	    edgelist.push_back(EdgeRef(this, vid, eci->first, *eci->second));
	    eci++;
	}

	if (eci != ecl.end() and eci->first == edges[edgeidx].target)
	{
	    // overwrite the edge object in the global graph
	    edgelist.push_back(EdgeRef(this, vid, eci->first, *eci->second));
	    eci++;
	    edgeidx++; // by skipping this entry
	}

	// add edges from the global graph
	while (edgeidx != endedgeidx and (eci == ecl.end() or eci->first > edges[edgeidx].target))
	{
	    edgelist.push_back(EdgeRef(this, vid, edgeidx));
	    edgeidx++;
	}
    }

    // add rest edges that come after the last index.
    while (eci != ecl.end())
    {
	edgelist.push_back(EdgeRef(this, vid, eci->first, *eci->second));
	eci++;
    }

    assert(eci == ecl.end());

    return edgelist;
}

void GraphContainer::getGraphProperties(class ByteBuffer &bb) const
{
    ByteOutBuffer s (bb);

    s.append<unsigned int>(0x00010000); // format v1.00

    properties.appendBinaryFormat(s);
}

static inline bool isInRectangle(coord_t x, coord_t y, coord_t px, coord_t py, coord_t qx, coord_t qy)
{
    return (px <= x and x <= qx) and (py <= y and y <= qy);
}

/*
Output byte stream: look at docs/timo/kodierung.pdf
*/

void GraphContainer::getArea(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
			     const QueryLimits *ql,
			     const char* filtertext, const char* vertexattrsel, const char* edgeattrsel,
			     const class ChangeTimeline &ct, class ByteBuffer &dest) const
{
#ifdef DBGOUTPUT
    double ts1 = timestamp();
#endif

    ByteOutBuffer s (dest);
    s.append<unsigned int>(0x00010000); // format v1.00

    // *** parse selection expressions
    AttributeSelectorList asl_vertex, asl_edge;
    
    // vertex selection expression
    try {
	if (!vertexattrsel) vertexattrsel = "";
	asl_vertex.parseString(vertexattrsel, properties, VE_VERTEX);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // edge selection expression
    try {
	if (!edgeattrsel) edgeattrsel = "";
	asl_edge.parseString(edgeattrsel, properties, VE_EDGE);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // parse filter expression
    FilterRoot filter;
    
    try {
	if (!filtertext) filtertext = "";
	filter.parseString(filtertext, properties);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // append section 1 describing the graph vertex attribute properties parsed
    // from the vertexattrsel
    s.append<unsigned char>(0x01);
    asl_vertex.appendBinaryFormat(s);

    // append section 2 describing the graph edge attribute properties parsed
    // from the edgeattrsel
    s.append<unsigned char>(0x02);
    asl_edge.appendBinaryFormat(s);

    // *** perform range query on the r-trees

    bool selvertex = asl_vertex.size() != 0;
    bool seledge = asl_edge.size() != 0;

    if (selvertex or seledge)
    {
	RTree_t::Rect queryrect(std::min(x1,x2), std::min(y1,y2),
				std::max(x1,x2), std::max(y1,y2));

	rtreemap_t::const_iterator rtreelevel = rtreemap.begin();

	// set to remove duplicate vertex info from result
	hash_set<unsigned int> vertexdup;

	// query limit parameter
	unsigned int vertexmaxlimit = ql ? ql->vertexmaxlimit : 400000;
	unsigned int vertexminlimit = ql ? ql->vertexminlimit : 100000;
	unsigned int edgemaxlimit = ql ? ql->edgemaxlimit : 400000;
	unsigned int edgeminlimit = ql ? ql->edgeminlimit : 100000;

	// two different counters here: first one counts all "seen" objects, so
	// this will become == maxlimit when enough are seen, and resultnum
	// which is the number of picked objects.

	unsigned int vertexnum = 0;
	unsigned int edgenum = 0;

	unsigned int vertexresultnum = 0;
	unsigned int edgeresultnum = 0;

	// this is used as std::stack, but we also need a reverse_iterator at
	// the end.
	typedef std::deque<Visitor> levels_t;
	std::deque<Visitor> levels;
	
	// TODO: fix lookups into changelist.

	while(rtreelevel != rtreemap.end() &&
	      ( (selvertex && vertexnum < vertexminlimit) ||
		(seledge && edgenum < edgeminlimit) )
	     )
	{
#ifdef DBGOUTPUT
	    std::cerr << "Query on level " << rtreelevel->first << " with vmax "
		      << (vertexmaxlimit - vertexnum) << " and emax "
		      << (edgemaxlimit - edgenum) << ".\n";
#endif

	    // create new visitor
	    levels.push_back( Visitor(*this, asl_vertex, asl_edge, filter, ct.getChangelistStart(),
				      vertexmaxlimit - vertexnum,
				      edgemaxlimit - edgenum,
				      vertexdup) );

	    Visitor &v = levels.back();
	    // query the rtree in the right level.
	    rtreelevel->second.queryRange(queryrect, v);

#ifdef DBGOUTPUT
	    std::cerr << "Level " << rtreelevel->first << " yielded "
		      << v.vertexnum << " vertices and " << v.edgenum << " edges\n";
#endif

	    if ((!selvertex || vertexnum + v.vertexnum >= vertexmaxlimit) &&
		(!seledge   || edgenum + v.edgenum >= edgemaxlimit))
	    {
#ifdef DBGOUTPUT
		std::cerr << "Overflow. Removing last level.\n";
#endif
		levels.pop_back();
		break;
	    }
	    else
	    {
		// continue into next level
		vertexnum += v.vertexnum;
		edgenum += v.edgenum;

		if (vertexresultnum + v.vertexnum <= vertexmaxlimit)
		    vertexresultnum += v.vertexnum;

		if (edgeresultnum + v.edgenum <= edgemaxlimit)
		    edgeresultnum += v.edgenum;

		rtreelevel++;
	    }
	}

	assert(vertexresultnum <= vertexmaxlimit);
	assert(edgeresultnum <= edgemaxlimit);

	// append each of the selected levels in reverse order so that the
	// nodes with highest z-value (lowest priority) come at the end.

	if (selvertex)
	{
	    // append section 3 which are the selected vertices.
	    s.append<unsigned char>(0x03);

	    s.append<unsigned int>(vertexresultnum);

	    unsigned int vertexcount = 0;
	    for(levels_t::reverse_iterator li = levels.rbegin(); li != levels.rend(); ++li)
	    {
		s.appendByteBuffer(li->vertexout);

		vertexcount += li->vertexnum;
	    }
	    assert(vertexcount == vertexresultnum);
	}

	if (seledge)
	{
	    // append section 4 which are the selected edges.
	    s.append<unsigned char>(0x04);

	    s.append<unsigned int>(edgeresultnum);

	    unsigned int edgecount = 0;

	    for(levels_t::reverse_iterator li = levels.rbegin(); li != levels.rend(); ++li)
	    {
		s.appendByteBuffer(li->edgeout);
		edgecount += li->edgenum;
	    }

	    assert(edgecount == edgeresultnum);
	}
	lastqueryedgenum = edgeresultnum;
    }

    if ((selvertex or seledge) and ct.getFrameCount() > 0)
    {
	// append animation section 5
	s.append<unsigned char>(0x05);

	// temp save the animation data, so we can add it's size
	ByteBuffer abb;
	ByteOutBuffer abob (abb);

	// number of animation frames following
	abob.append<unsigned int>(ct.getFrameCount());

	// we have to keep an own Changelist of the current animation so
	// src.attr lookups will work right!
	Changelist anichg(*this);
	
	unsigned int p;
	AttributeVertexTinyBlob attrblob;

#ifdef DBGOUTPUT
	unsigned int anientry = 0;
	double ats1 = timestamp();
#endif

	for(ChangeTimeline::changeframelist_t::const_iterator cf = ct.frames.begin(); cf != ct.frames.end(); ++cf)
	{
	    abob.append<unsigned int>(cf->chgoplist.size());
	    
	    for(ChangeFrame::chgoplist_t::const_iterator cfe = cf->chgoplist.begin(); cfe != cf->chgoplist.end(); ++cfe)
	    {
		// save the change operation made
		abob.append<unsigned char>(cfe->chgid);

#ifdef DBGOUTPUT
		anientry++;
#endif
		
		// apply the change to our running ChangeList and save the
		// selector's result on the changed vertex/edge
		switch(cfe->chgid)
		{
		case ChangeFrame::CHG_NONE:
		default:
		    assert(0);
		    break;

		// *** Vertex Operations

		case ChangeFrame::CHG_ADDVERTEX:
		    anichg.addVertex(cfe->vid1, cfe->blob);

		    p = asl_vertex.processVertexAttributeBlob(attrblob, *this, anichg, cfe->vid1);
		    abob.appendBytes(attrblob.data(), p);

		    break;

		case ChangeFrame::CHG_SETVERTEX:

		    p = asl_vertex.processVertexAttributeBlob(attrblob, *this, anichg, cfe->vid1);
		    abob.appendBytes(attrblob.data(), p);

		    anichg.setVertexAttr(cfe->vid1, cfe->blob);

		    p = asl_vertex.processVertexAttributeBlob(attrblob, *this, anichg, cfe->vid1);
		    abob.appendBytes(attrblob.data(), p);

		    break;

		case ChangeFrame::CHG_DELVERTEX:

		    p = asl_vertex.processVertexAttributeBlob(attrblob, *this, anichg, cfe->vid1);
		    abob.appendBytes(attrblob.data(), p);

		    anichg.delVertex(cfe->vid1);
		    break;

		// *** Edge Operations

		case ChangeFrame::CHG_ADDEDGE:
		    anichg.addEdge(cfe->vid1, cfe->vid2, AttributeEdgeTinyBlob(cfe->blob.data(), cfe->blob.size()));

		    p = asl_edge.processEdgeAttributeBlob(attrblob, *this, anichg, cfe->vid1, cfe->vid2);
		    abob.appendBytes(attrblob.data(), p);

		    break;

		case ChangeFrame::CHG_SETEDGE:
		    p = asl_edge.processEdgeAttributeBlob(attrblob, *this, anichg, cfe->vid1, cfe->vid2);
		    abob.appendBytes(attrblob.data(), p);

		    anichg.setEdgeAttr(cfe->vid1, cfe->vid2, reinterpret_cast<const AttributeEdgeTinyBlob&>(cfe->blob));

		    p = asl_edge.processEdgeAttributeBlob(attrblob, *this, anichg, cfe->vid1, cfe->vid2);
		    abob.appendBytes(attrblob.data(), p);

		    break;

		case ChangeFrame::CHG_DELEDGE:

		    p = asl_edge.processEdgeAttributeBlob(attrblob, *this, anichg, cfe->vid1, cfe->vid2);
		    abob.appendBytes(attrblob.data(), p);

		    anichg.delEdge(cfe->vid1, cfe->vid2);
		    break;
    		}
	    }
	}
	
// append the size of the animation data
#ifdef DBGOUTPUT
	double ats2 = timestamp();

	std::cerr << "Animation size: " << abb.size() << " - " << anientry << " entries\n";
	std::cerr << "animation time: " << (ats2 - ats1) << "\n";
#endif

	s.append<unsigned int>(abb.size());
	s.appendByteBuffer(abb);
    }

    // terminating section id
    s.append<unsigned char>(0x00);

#ifdef DBGOUTPUT
    double ts2 = timestamp();

    std::cerr << "getArea Querytime: " << (ts2 - ts1) << "sec. Result size " << dest.size() << "\n";
#endif
}

inline GraphContainer::Visitor::Visitor(const GraphContainer &_gc,
	const class AttributeSelectorList &_asl_vertex, const class AttributeSelectorList &_asl_edge,
	const class FilterRoot &_filter,
	const class Changelist &_cl, unsigned int _vertexmaxlimit, unsigned int _edgemaxlimit,
	hash_set<unsigned int> &_vertexdup)
    : gc(_gc),
      asl_vertex(_asl_vertex), asl_edge(_asl_edge),
      filter(_filter),
      cl(_cl),
      vertexmaxlimit(_vertexmaxlimit), edgemaxlimit(_edgemaxlimit),
      vertexdup(_vertexdup),
      vertexnum(0), edgenum(0)
{
    if (filter.isEmpty()) filtertype = 0;
    else if (filter.getType() == VE_VERTEX) filtertype = 1;
    else filtertype = 2;
}

inline void GraphContainer::Visitor::add_vertex(unsigned int vid)
{
    // check in vertex duplicate hash
    if (vertexdup.insert(vid).second == false)
    {
	// insertion failed -> vertex id is already in the set.
	return;
    }

#ifdef CALCATTRVALUES
    // add vertex attributes to output buffer
    unsigned int p = asl_vertex.processVertexAttributeBlob(attrblob, gc, cl, vid);

    ByteOutBuffer vertexoutwr (vertexout);

    vertexoutwr.appendBytes(attrblob.data(), p);

#else
#warning "Attribute calculation is disabled!"
#endif
    vertexnum++;
}

inline void GraphContainer::Visitor::add_edge(unsigned int src, unsigned int tgt)
{
#ifdef CALCATTRVALUES
    // add edge attributes to output buffer
    unsigned int p = asl_edge.processEdgeAttributeBlob(attrblob, gc, cl, src, tgt);

    ByteOutBuffer edgeoutwr (edgeout);

    edgeoutwr.appendBytes(attrblob.data(), p);
#endif
    edgenum++;
}

bool GraphContainer::Visitor::operator()(const RTree_t::Rect &, const RTreeData &d)
{
    // TODO: add function to exclude edges not in range
    // TODO: changelist

    if (filtertype == 0) // null-filter
    {
	if (asl_edge.size() != 0 && (edgenum < edgemaxlimit))
	{
	    add_edge(d.src, d.tgt);
	}

	if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
	{
	    add_vertex(d.src);
	}

	if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
	{
	    add_vertex(d.tgt);
	}
    }
    else if (filtertype == 1) // vertex-filter
    {
	bool addedge = false;

	// check if the filter is true on the source vertex of this edge
	if (filter.eval_vertex(gc, cl, d.src))
	{
	    if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
		add_vertex(d.src);

	    addedge = true;
	}
	
	// then the target vertex
	if (filter.eval_vertex(gc, cl, d.tgt))
	{
	    if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
		add_vertex(d.tgt);

	    addedge = true;
	}
	
	// add the edge inbetween if either vertex was included
	if (asl_edge.size() != 0 && (edgenum < edgemaxlimit) && addedge)
	{
	    add_edge(d.src, d.tgt);
	}
    }
    else if (filtertype == 2) // edge-filter
    {
	// check if the filter is true on the edge
	if (filter.eval_edge(gc, cl, d.src, d.tgt))
	{
	    if (asl_edge.size() != 0 && (edgenum < edgemaxlimit))
		add_edge(d.src, d.tgt);

	    // add inzident vertices if there is still room.
	    if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.src);
	    }

	    if (asl_vertex.size() != 0 && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.tgt);
	    }
	}
    }

    // stop the range query first when both counters exceed their maxlimit.
    return (edgenum < edgemaxlimit) || (vertexnum < vertexmaxlimit);
}

void GraphContainer::getNearestNeighbor(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
					coord_t selx, coord_t sely, const QueryLimits *ql,
					const char* filtertext, const char* vertexattrsel, const char* edgeattrsel,
					const class Changelist &cl, class ByteBuffer &dest) const
{
#ifdef DBGOUTPUT
    double ts1 = timestamp();
#endif

    ByteOutBuffer s (dest);
    s.append<unsigned int>(0x00010000); // format v1.00

    // *** parse selection expressions
    AttributeSelectorList asl_vertex, asl_edge;
    
    // vertex selection expression
    try {
	if (!vertexattrsel) vertexattrsel = "";
	asl_vertex.parseString(vertexattrsel, properties, VE_VERTEX);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // edge selection expression
    try {
	if (!edgeattrsel) edgeattrsel = "";
	asl_edge.parseString(edgeattrsel, properties, VE_EDGE);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // parse filter expression
    FilterRoot filter;
    
    try {
	if (!filtertext) filtertext = "";
	filter.parseString(filtertext, properties);
    }
    catch (GraphException &e) // these are ParseException or ConversionException
    {
	// output string error into buffer
	s.append<unsigned char>(0xFF);
	s.append<unsigned char>(static_cast<unsigned char>(e.what_str().size()));
	s.appendString(e.what_str());
	return;
    }

    // append section 1 describing the graph vertex attribute properties parsed
    // from the vertexattrsel
    s.append<unsigned char>(0x01);
    asl_vertex.appendBinaryFormat(s);

    // append section 2 describing the graph edge attribute properties parsed
    // from the edgeattrsel
    s.append<unsigned char>(0x02);
    asl_edge.appendBinaryFormat(s);

    // *** perform range query on the r-trees

    bool selvertex = asl_vertex.size() != 0;
    bool seledge = asl_edge.size() != 0;

    if (selvertex or seledge)
    {
	RTree_t::Rect queryrect(std::min(x1,x2), std::min(y1,y2),
				std::max(x1,x2), std::max(y1,y2));

	rtreemap_t::const_iterator rtreelevel = rtreemap.begin();

	// set to remove duplicate vertex info from result
	hash_set<unsigned int> vertexdup;

	// query limit parameters
	unsigned int vertexmaxlimit = ql ? ql->vertexmaxlimit : 400000;
	unsigned int vertexminlimit = ql ? ql->vertexminlimit : 100000;
	unsigned int edgemaxlimit = ql ? ql->edgemaxlimit : 400000;
	unsigned int edgeminlimit = ql ? ql->edgeminlimit : 100000;

	// two different counters here: first one counts all "seen" objects, so
	// this will become == maxlimit when enough are seen, and resultnum
	// which is the number of picked objects.

	unsigned int vertexnum = 0;
	unsigned int edgenum = 0;

	unsigned int vertexresultnum = 0;
	unsigned int edgeresultnum = 0;

	// this is used as std::stack, but we also need a reverse_iterator at
	// the end.
	typedef std::deque<NNVisitor> levels_t;
	std::deque<NNVisitor> levels;

	/// this is the result nearest neighbor object
	NNResult nnr( RTree_t::Point(selx, sely) );
	
	// TODO: fix lookups into changelist.

	while(rtreelevel != rtreemap.end() &&
	      ( (selvertex && vertexnum < vertexminlimit) ||
		(seledge && edgenum < edgeminlimit) )
	     )
	{
#ifdef DBGOUTPUT
	    std::cerr << "Query on level " << rtreelevel->first << " with vmax "
		      << (vertexmaxlimit - vertexnum) << " and emax "
		      << (edgemaxlimit - edgenum) << ".\n";
#endif

	    // create new visitor
	    levels.push_back( NNVisitor(*this, nnr, asl_vertex, asl_edge, filter, cl,
					vertexmaxlimit - vertexnum,
					edgemaxlimit - edgenum,
					vertexdup) );

	    NNVisitor &v = levels.back();
	    // query the rtree in the right level.
	    rtreelevel->second.queryRange(queryrect, v);

#ifdef DBGOUTPUT
	    std::cerr << "Level " << rtreelevel->first << " yielded "
		      << v.vertexnum << " vertices and " << v.edgenum << " edges\n";
#endif

	    if ((!selvertex || vertexnum + v.vertexnum >= vertexmaxlimit) &&
		(!seledge   || edgenum + v.edgenum >= edgemaxlimit))
	    {
#ifdef DBGOUTPUT
		std::cerr << "Overflow. Removing last level.\n";
#endif
		levels.pop_back();
		// do not update nnr object.
		break;
	    }
	    else
	    {
		// continue into next level
		vertexnum += v.vertexnum;
		edgenum += v.edgenum;

		if (vertexresultnum + v.vertexnum <= vertexmaxlimit)
		{
		    vertexresultnum += v.vertexnum;

		    // copy vertex nnr result
		    nnr.nn_vid = v.nnr.nn_vid;
		    nnr.dist_nn_vid = v.nnr.dist_nn_vid;
		}

		if (edgeresultnum + v.edgenum <= edgemaxlimit)
		{
		    edgeresultnum += v.edgenum;

		    // copy edge nnr result
		    nnr.nn_esrc = v.nnr.nn_esrc;
		    nnr.nn_etgt = v.nnr.nn_etgt;
		    nnr.dist_nn_edge = v.nnr.dist_nn_edge;
		}

		rtreelevel++;
	    }
	}

	assert(vertexresultnum <= vertexmaxlimit);
	assert(edgeresultnum <= edgemaxlimit);

	// nearestNeighbor returns only one object each, if it was found

	AttributeVertexTinyBlob attrblob;

	if (selvertex && nnr.nn_vid != VERTEX_INVALID)
	{
	    // append section 3 which are the selected vertices.
	    s.append<unsigned char>(0x03);

	    s.append<unsigned int>(1);

	    unsigned int p = asl_vertex.processVertexAttributeBlob(attrblob, *this, cl, nnr.nn_vid);
	    s.appendBytes(attrblob.data(), p);
	}

	if (seledge && nnr.nn_esrc != VERTEX_INVALID)
	{
	    // append section 4 which are the selected edges.
	    s.append<unsigned char>(0x04);

	    s.append<unsigned int>(1);

	    unsigned int p = asl_edge.processEdgeAttributeBlob(attrblob, *this, cl, nnr.nn_esrc, nnr.nn_etgt);
	    s.appendBytes(attrblob.data(), p);
	}
    }

    // terminating section id
    s.append<unsigned char>(0x00);

#ifdef DBGOUTPUT
    double ts2 = timestamp();

    std::cerr << "getNearestNeighbor Querytime: " << (ts2 - ts1) << "sec. Result size " << dest.size() << "\n";
#endif
}

inline GraphContainer::NNVisitor::NNVisitor(const GraphContainer &_gc, const class NNResult &_nnr,
	const class AttributeSelectorList &_asl_vertex, const class AttributeSelectorList &_asl_edge,
	const class FilterRoot &_filter,
	const class Changelist &_cl, unsigned int _vertexmaxlimit, unsigned int _edgemaxlimit,
	hash_set<unsigned int> &_vertexdup)
    : gc(_gc),
      use_asl_vertex(_asl_vertex.size() != 0), use_asl_edge(_asl_edge.size() != 0),
      filter(_filter),
      cl(_cl),
      vertexmaxlimit(_vertexmaxlimit), edgemaxlimit(_edgemaxlimit),
      vertexdup(_vertexdup),
      vertexnum(0), edgenum(0),
      nnr(_nnr)
{
    if (filter.isEmpty()) filtertype = 0;
    else if (filter.getType() == VE_VERTEX) filtertype = 1;
    else filtertype = 2;
}

/// initializer
inline GraphContainer::NNResult::NNResult(const RTree_t::Point &p)
{
    point = p;

    nn_vid = VERTEX_INVALID;
    nn_esrc = nn_etgt = VERTEX_INVALID;

    dist_nn_vid = std::numeric_limits<RTree_t::AreaType>::max();
    dist_nn_edge = std::numeric_limits<RTree_t::AreaType>::max();
}

inline void GraphContainer::NNResult::checkVertex(coord_t x, coord_t y, unsigned int vid)
{
    RTree_t::AreaType d = point.getDistanceSquare(x, y);

    if (d < dist_nn_vid)
    {
	nn_vid = vid;
	dist_nn_vid = d;
    }
}

inline void GraphContainer::NNResult::checkEdge(coord_t x1, coord_t y1, coord_t x2, coord_t y2, unsigned int vsrc, unsigned int vtgt)
{
    RTree_t::AreaType d = point.getDistanceLineSquare(x1, y1, x2, y2);

    if (d < dist_nn_edge)
    {
	nn_esrc = vsrc;
	nn_etgt = vtgt;
	dist_nn_edge = d;
    }
}

inline void GraphContainer::NNVisitor::add_vertex(unsigned int vid)
{
    // check in vertex duplicate hash
    if (vertexdup.insert(vid).second == false)
    {
	// insertion failed -> vertex id is already in the set.
	return;
    }

    vertexnum++;
}

inline void GraphContainer::NNVisitor::add_edge(unsigned int , unsigned int )
{
    edgenum++;
}

bool GraphContainer::NNVisitor::operator()(const RTree_t::Rect &, const RTreeData &d)
{
    // TODO: changelist

    // retrieve coordinates. this could be accelerated by using the rectangle,
    // but we dont know how the coordinates correspond.
    coord_t x1 = gc.getVertexAttr(d.src, gc.vertexattrid_xaxis).getInteger();
    coord_t y1 = gc.getVertexAttr(d.src, gc.vertexattrid_yaxis).getInteger();

    coord_t x2 = gc.getVertexAttr(d.tgt, gc.vertexattrid_xaxis).getInteger();
    coord_t y2 = gc.getVertexAttr(d.tgt, gc.vertexattrid_yaxis).getInteger();


    if (filtertype == 0) // null-filter
    {
	if (use_asl_edge && (edgenum < edgemaxlimit))
	{
	    add_edge(d.src, d.tgt);
	    nnr.checkEdge(x1, y1, x2, y2, d.src, d.tgt);
	}

	if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	{
	    add_vertex(d.src);
	    nnr.checkVertex(x1, y1, d.src);
	}

	if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	{
	    add_vertex(d.tgt);
	    nnr.checkVertex(x2, y2, d.tgt);
	}
    }
    else if (filtertype == 1) // vertex-filter
    {
	bool addedge = false;

	// check if the filter is true on the source vertex of this edge
	if (filter.eval_vertex(gc, cl, d.src))
	{
	    if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.src);
		nnr.checkVertex(x1, y1, d.src);
	    }

	    addedge = true;
	}
	
	// then the target vertex
	if (filter.eval_vertex(gc, cl, d.tgt))
	{
	    if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.tgt);
		nnr.checkVertex(x1, y1, d.tgt);
	    }

	    addedge = true;
	}
	
	// add the edge inbetween if either vertex was included
	if (use_asl_edge && (edgenum < edgemaxlimit) && addedge)
	{
	    add_edge(d.src, d.tgt);
	    nnr.checkEdge(x1, y1, x2, y2, d.src, d.tgt);
	}
    }
    else if (filtertype == 2) // edge-filter
    {
	// check if the filter is true on the edge
	if (filter.eval_edge(gc, cl, d.src, d.tgt))
	{
	    if (use_asl_edge && (edgenum < edgemaxlimit))
	    {
		add_edge(d.src, d.tgt);
		nnr.checkEdge(x1, y1, x2, y2, d.src, d.tgt);
	    }

	    // add inzident vertices if there is still room.
	    if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.src);
		nnr.checkVertex(x1, y1, d.src);
	    }

	    if (use_asl_vertex && (vertexnum < vertexmaxlimit))
	    {
		add_vertex(d.tgt);
		nnr.checkVertex(x1, y1, d.tgt);
	    }
	}
    }

    // stop the range query first when both counters exceed their maxlimit.
    return (edgenum < edgemaxlimit) || (vertexnum < vertexmaxlimit);
}

} // namespace VGServer
