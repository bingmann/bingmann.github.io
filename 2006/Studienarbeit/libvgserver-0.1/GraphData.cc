// $Id: GraphData.cc 241 2006-07-05 07:29:52Z bingmann $

#include <map>
#include <assert.h>
#include <iostream>

#include "GraphData.h"
#include "Changelist.h"
#include "ByteOutBuffer.h"

#include "AttributeBlob_impl.h"

namespace VGServer {

GraphData::GraphData(const class GraphProperties &_properties)
    : properties(_properties)
{
    // initialize an empty graph: just the termination vertex and edge objects.
    Vertex &v = vertices.new_back();
    v.attridx = v.edgeidx = 0;

    vertexcount = 0;

    edges.new_back();

    // do attribute lookups calculation
    properties.calcAttributeLookups();
}

void GraphData::swap(class GraphData &other)
{
    std::swap(properties, other.properties);
    
    vertices.swap(other.vertices);
    edges.swap(other.edges);
    
    vertexattr.swap(other.vertexattr);
    edgeattr.swap(other.edgeattr);

    std::swap(vertexcount, other.vertexcount);
}

//#define DBG_APPLYCHGLIST

void GraphData::applyChangelist(const class Changelist &cl)
{
    assert( checkReferences() );

    // first straight-forward implementation: create a new arrays, and fills
    // them with the merged data.

    TpArray<Edge> newedges;

    AttributeBigBlob newvertexattr;
    AttributeBigBlob newedgeattr;

    // ** Prepare data from the Changelist, by sorting it using std::map, but
    // ** do not copy the AttributeBlobs.
    typedef std::map<vertexid_t, const AttributeVertexTinyBlob*> vertexchglist_sorted_t;
    vertexchglist_sorted_t vertexchg;

    for(Changelist::vertexchglist_t::const_iterator vci = cl.vertexchglist.begin(); vci != cl.vertexchglist.end(); ++vci)
    {
	vertexchg[vci->first] = &vci->second;
    }

    // this will also sort the key type which is a pair.
    typedef std::map<Changelist::vertexidpair_t, const AttributeEdgeTinyBlob*> edgechglist_sorted_t;
    edgechglist_sorted_t edgechg;

    for(Changelist::edgechglist_t::const_iterator eci = cl.edgechglist.begin(); eci != cl.edgechglist.end(); ++eci)
    {
	edgechg[eci->first] = &eci->second;
    }

#ifdef DBG_APPLYCHGLIST
    // print out changes

    std::cout << "Vertex Changes:\n";
    for(vertexchglist_sorted_t::const_iterator vci = vertexchg.begin(); vci != vertexchg.end(); ++vci)
    {
	std::cout << "v: " << vci->first << "\n";
    }

    std::cout << "Edge Changes:\n";
    for(edgechglist_sorted_t::const_iterator eci = edgechg.begin(); eci != edgechg.end(); ++eci)
    {
	std::cout << "e: " << eci->first.first << "," << eci->first.second << "\n";
    }
#endif

    // *** reserve size in vertex list for the maximum vertex id in the change
    // *** list, and create blank vertex objects all the way up to the new
    // *** maximum id.
    vertexid_t oldmaxvnum = vertices.size()-1;
    vertexid_t newmaxvnum = vertexchg.empty() ? oldmaxvnum : vertexchg.rbegin()->first+1;

    if (oldmaxvnum < newmaxvnum)
    {
	vertices.reserve(newmaxvnum + 1); // maximum vertex num + termination

	assert(oldmaxvnum < vertices.size());
	unsigned int attrendidx = vertices[oldmaxvnum].attridx;

	// fill new vertex.edgeidx with the maximum boundary, dont overwrite
	// the old termination vertex object, nor write the new termination,
	// but insert it.
	for(vertexid_t vi = oldmaxvnum+1; vi < newmaxvnum+1; vi++)
	{
	    Vertex &v = vertices.new_back();
	    v.edgeidx = edges.size()-1;
	    v.attridx = attrendidx;
	}

	assert(vertices.size() == newmaxvnum+1);
    }

    // calculate the maximum size of the new edges vector
    newedges.reserve(edges.size() + edgechg.size());

    // merge together the old arrays and the sorted changelists
    vertexchglist_sorted_t::const_iterator vertexchgiter = vertexchg.begin();
    edgechglist_sorted_t::const_iterator edgechgiter = edgechg.begin();

    unsigned int vertexiter = 0;
    unsigned int oldedgeiter = 0;
    unsigned int newedgeiter = 0;

    unsigned int newvertexattrpos = 0;
    unsigned int newedgeattrpos = 0;

    unsigned int newvertexcount = 0;

    while( vertexiter < vertices.size()-1 )
    {
	// *** Merge Vertex Attributes

	if (vertexchgiter != vertexchg.end() and vertexiter == vertexchgiter->first)
	{
	    // vertexiter id matches the next vertex change: use attribute
	    // value chain from the Changelist
	    
	    const AttributeVertexTinyBlob* ap = vertexchgiter->second;

	    unsigned int attrlen = ap->getAttrChainLength(0, properties.vertexattr);

	    newvertexattr.copy(newvertexattrpos, ap->data(), attrlen);

	    vertices[vertexiter].attridx = newvertexattrpos;

	    newvertexattrpos += attrlen;

	    if (attrlen > 0) newvertexcount++;

	    vertexchgiter++;
	}
	else
	{
	    // copy old attribute info
	    
	    unsigned int attrlen = vertices[vertexiter+1].attridx - vertices[vertexiter].attridx;

	    if (attrlen > 0)
		newvertexattr.copy(newvertexattrpos, vertexattr.data(vertices[vertexiter].attridx), attrlen);

	    vertices[vertexiter].attridx = newvertexattrpos;

	    newvertexattrpos += attrlen;

	    if (attrlen > 0) newvertexcount++;
	}

	vertices[vertexiter].edgeidx = newedgeiter;
	assert(newedgeiter == newedges.size());

	// *** Merge Edge Attributes

	if (vertices[vertexiter].attridx != newvertexattrpos) // checks if this vertex is not blank
	{
	    // iterate until the next vertex's edges begin.
	    unsigned int endedgeidx = vertices[vertexiter+1].edgeidx;

	    while(edgechgiter != edgechg.end() and edgechgiter->first.first < vertexiter) {
		//assert(0); // somehow this should never occur. as it means that
			     // a source vertex id was skipped. Ah it occurs for
			     // deleted vertices.
		edgechgiter++;
	    }

	    if (edgechgiter == edgechg.end() or edgechgiter->first.first != vertexiter)
	    {
		// no more edge change objects apply to this vertex id. so copy
		// rest of existing edges.
		while(oldedgeiter < endedgeidx)
		{
		    assert( oldedgeiter < edges.size() );
		    
		    Edge &e = newedges.new_back();
		    e.target = edges[oldedgeiter].target;
		    
#ifdef DBG_APPLYCHGLIST
		    std::cout << "Flushing " << vertexiter << "," << e.target << "\n";
#endif

		    // copy edge attribute data
		    unsigned int attrlen = edges[oldedgeiter+1].attridx - edges[oldedgeiter].attridx;

		    if (attrlen > 0)
			newedgeattr.copy(newedgeattrpos, edgeattr.data(edges[oldedgeiter].attridx), attrlen);

		    e.attridx = newedgeattrpos;

		    newedgeattrpos += attrlen;

		    // increment edge array iterators
		    oldedgeiter++;
		    newedgeiter++;
		}
	    }
	    else
	    {
		// this edge change's source vertex matches!
		while(oldedgeiter < endedgeidx or (edgechgiter != edgechg.end() and edgechgiter->first.first == vertexiter))
		{
		    if (oldedgeiter >= endedgeidx
			or (edgechgiter != edgechg.end()
			    and edgechgiter->first.first == vertexiter
			    and edgechgiter->first.second < edges[oldedgeiter].target)
			)
		    {
			// this edge change matches, and inserts a new edge
			// into the global graph.
			if (not edgechgiter->second->empty())
			{
			    Edge &e = newedges.new_back();
			    e.target = edgechgiter->first.second;

#ifdef DBG_APPLYCHGLIST
			    std::cout << "InsertingNew " << vertexiter << "," << e.target << "\n";
#endif

			    // copy attribute chain from edgechg
			    const AttributeEdgeTinyBlob* ap = edgechgiter->second;

			    unsigned int attrlen = ap->getAttrChainLength(0, properties.edgeattr);

			    newedgeattr.copy(newedgeattrpos, ap->data(), attrlen);
			    
			    e.attridx = newedgeattrpos;

			    newedgeattrpos += attrlen;
			    newedgeiter++;
			}
			// else the change was deleted again.

			edgechgiter++;
		    }
		    else if (edgechgiter == edgechg.end()
			     or edgechgiter->first.first != vertexiter
			     or edgechgiter->first.second > edges[oldedgeiter].target)
		    {
			// no edge change matches this vertex id, so copy old
			// edge attributes

			assert( oldedgeiter < edges.size() );

			Edge &e = newedges.new_back();
			e.target = edges[oldedgeiter].target;

#ifdef DBG_APPLYCHGLIST
			std::cout << "OldCopy " << vertexiter << "," << e.target << "\n";
#endif

			// copy edge attribute data
			unsigned int attrlen = edges[oldedgeiter+1].attridx - edges[oldedgeiter].attridx;

			if (attrlen > 0)
			    newedgeattr.copy(newedgeattrpos, edgeattr.data(edges[oldedgeiter].attridx), attrlen);

			e.attridx = newedgeattrpos;

			newedgeattrpos += attrlen;

			oldedgeiter++;
			newedgeiter++;
		    }
		    else // edges[oldedgeiter].target == edgechgiter->first.second (modifiy existing edge)
		    {
			assert( edgechgiter->first.first == vertexiter );

			if (not edgechgiter->second->empty())
			{
			    Edge &e = newedges.new_back();
			    e.target = edgechgiter->first.second;

#ifdef DBG_APPLYCHGLIST
			    std::cout << "Modify " << vertexiter << "," << e.target << "\n";
#endif

			    // copy attribute chain from edgechg
			    const AttributeEdgeTinyBlob* ap = edgechgiter->second;

			    unsigned int attrlen = ap->getAttrChainLength(0, properties.edgeattr);

			    newedgeattr.copy(newedgeattrpos, ap->data(), attrlen);
			    
			    e.attridx = newedgeattrpos;

			    newedgeattrpos += attrlen;
			    newedgeiter++;
			}
			// else: this change was a delete modification, so no
			// new edge written

			oldedgeiter++;
			edgechgiter++;
		    }
		}
	    }
	}

	vertexiter++;
    }

    // write termination vertex
    vertices[vertices.size()-1].edgeidx = newedgeiter;
    vertices[vertices.size()-1].attridx = newvertexattrpos;

    // termination edge
    Edge &eend = newedges.new_back();
    eend.target = VERTEX_INVALID;
    eend.attridx = newedgeattrpos;

    assert( vertexchgiter == vertexchg.end() ); // otherwise an id in the changes is > new maxvertexid.

    // free unused data.
    //newedges.resize(newedgeiter);

    // move new arrays into the object
    edges.swap( newedges );
    vertexattr.swap( newvertexattr );
    edgeattr.swap( newedgeattr );

    vertexcount = newvertexcount;

    assert( checkReferences() );
}

void GraphData::writeFig(bool writefigheader, std::ostream &o, unsigned int attrX, unsigned int attrY) const
{
    if (writefigheader)
	o << "#FIG 3.2\nLandscape\nCenter\nInches\nLetter\n100.00\nSingle\n-2\n1200 2\n";

    for(vertexid_t vi = 0; vi < vertices.size()-1; vi++)
    {
	if (not existVertex(vi)) continue;

	o << "1 4 0 1 0 7 50 -1 -1 0.000 1 0.0000 "
	  << getVertexAttr(vi, attrX).getString() << " "
	  << getVertexAttr(vi, attrY).getString() << " 32 32 0 0 0 0\n";

	unsigned int endedgeidx = vertices[vi+1].edgeidx;

	for(unsigned int ei = vertices[vi].edgeidx; ei < endedgeidx; ei++)
	{
	    vertexid_t tvi = edges[ei].target;
	    if (tvi == VERTEX_INVALID) break;
	    if (not existVertex(tvi)) continue;

	    o << "2 1 0 1 0 7 50 -1 -1 0.000 0 0 -1 0 0 2\n";
	    o << "\t\t"
	      << getVertexAttr(vi, attrX).getString() << " " << getVertexAttr(vi, attrY).getString() << " "
	      << getVertexAttr(tvi, attrX).getString() << " " << getVertexAttr(tvi, attrY).getString() << "\n";
	}
    }
}

#define funcassert(x)	do { if (!(x)) { fprintf(stderr, "assertion %s failed ", #x); return false; } } while(0)

bool GraphData::checkReferences() const
{
    for(vertexid_t vi = 0; vi < vertices.size()-1; vi++)
    {
	funcassert(vertices[vi].edgeidx <= vertices[vi+1].edgeidx);

	funcassert(vertices[vi].edgeidx <= edges.size());
	funcassert(vertices[vi+1].edgeidx <= edges.size());

	// check ascending edge targets
	for(unsigned int ei = vertices[vi].edgeidx; ei + 1 < vertices[vi+1].edgeidx; ei++)
	{
	    funcassert(edges[ei].target < edges[ei+1].target);
	}

	funcassert(vertices[vi].attridx <= vertices[vi+1].attridx);

	funcassert(vertices[vi].attridx <= vertexattr.size());
	funcassert(vertices[vi+1].attridx <= vertexattr.size());
    }

    for(unsigned int ei = 0; ei < edges.size()-1; ei++)
    {
	funcassert(edges[ei].attridx <= edges[ei+1].attridx);

	funcassert(edges[ei].attridx <= edgeattr.size());
	funcassert(edges[ei+1].attridx <= edgeattr.size());
    }

    funcassert(vertices[vertices.size()-1].edgeidx == edges.size()-1);

    return true;
}

const GraphData::Vertex* GraphData::getVertex(vertexid_t id) const
{
    if (id >= vertices.size()-1) id = vertices.size()-1;

    return &vertices[id];
}

bool GraphData::existVertex(vertexid_t id) const
{
    if (id >= vertices.size()-1) return false;

    return (vertices[id].attridx < vertices[id+1].attridx);
}

class AnyType GraphData::getVertexAttr(vertexid_t id, unsigned int attr) const
{
    const Vertex *v = getVertex(id);
    return v->getAttr(attr, *this);
}

AttributeVertexTinyBlob GraphData::getVertexAttrBlob(vertexid_t id) const
{
    if (id >= vertices.size()-1) return AttributeVertexTinyBlob(); // thus a non-existing vertex

    // copy the attribute value range from the blob
    return AttributeVertexTinyBlob(vertexattr.data(vertices[id].attridx),
				   vertices[id+1].attridx - vertices[id].attridx);
}

unsigned int GraphData::getEdgeIdx(vertexid_t src, vertexid_t tgt) const
{
    if (src >= vertices.size()-1) return edges.size()-1;
    if (tgt >= vertices.size()-1) return edges.size()-1;

    int startidx = vertices[src].edgeidx;
    int endidx = vertices[src+1].edgeidx;

    for(int ei = startidx; ei < endidx; ei++)
    {
	if (edges[ei].target == tgt) {
	    return ei;
	}
    }
    return edges.size()-1;
}


const GraphData::Edge* GraphData::getEdge(vertexid_t src, vertexid_t tgt) const
{
    if (src >= vertices.size()-1) return NULL;
    if (tgt >= vertices.size()-1) return NULL;

    int startidx = vertices[src].edgeidx;
    int endidx = vertices[src+1].edgeidx;

    for(int ei = startidx; ei < endidx; ei++)
    {
	if (edges[ei].target == tgt) {
	    return &edges[ei];
	}
    }
    return NULL;
}

bool GraphData::existEdge(vertexid_t src, vertexid_t tgt) const
{
    return getEdgeIdx(src,tgt) < (edges.size()-1);
}

class AnyType GraphData::getEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attr) const
{
    const Edge *e = getEdge(src,tgt);
    if (e == NULL) return AnyType(ATTRTYPE_INVALID);
    return e->getAttr(attr, *this);
}

AttributeEdgeTinyBlob GraphData::getEdgeAttrBlob(vertexid_t src, vertexid_t tgt) const
{
    if (src >= vertices.size()-1) return AttributeEdgeTinyBlob(); // thus a non-existing edge
    if (tgt >= vertices.size()-1) return AttributeEdgeTinyBlob();
  
    int startidx = vertices[src].edgeidx;
    int endidx = vertices[src+1].edgeidx;

    for(int ei = startidx; ei < endidx; ei++)
    {
	if (edges[ei].target == tgt)
	{
	    // copy the attribute value range from the blob
	    return AttributeEdgeTinyBlob(edgeattr.data(edges[ei].attridx),
					 edges[ei+1].attridx - edges[ei].attridx);
	}
    }
    return AttributeEdgeTinyBlob();
}

std::vector<const GraphData::Edge*> GraphData::getEdgeList(vertexid_t id) const
{
    std::vector<const Edge*> outedges;

    if (id >= vertices.size()-1) return outedges;

    int startidx = vertices[id].edgeidx;
    int endidx = vertices[id+1].edgeidx;

    outedges.resize(endidx - startidx);
    for(int ei = startidx; ei < endidx; ei++)
	outedges[ei - startidx] = &edges[ei];

    return outedges;
}

std::vector<unsigned int> GraphData::getEdgeIdxList(vertexid_t id) const
{
    std::vector<unsigned int> outedges;

    if (id >= vertices.size()-1) return outedges;

    int startidx = vertices[id].edgeidx;
    int endidx = vertices[id+1].edgeidx;

    for(int ei = startidx; ei < endidx; ei++)
	outedges.push_back(ei);

    return outedges;
}

std::vector<unsigned int> GraphData::getEdgeTargetList(vertexid_t id) const
{
    std::vector<unsigned int> outedges;

    if (id >= vertices.size()-1) return outedges;

    int startidx = vertices[id].edgeidx;
    int endidx = vertices[id+1].edgeidx;

    for(int ei = startidx; ei < endidx; ei++)
	outedges.push_back(edges[ei].target);

    return outedges;
}

void GraphData::saveSnapshot(std::ostream &s) const
{
    // first write a signature
    unsigned int key = 0x12121212;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    // write the properties
    key = 0x00000001;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    ByteBuffer bb;
    ByteOutBuffer bob(bb);

    properties.appendBinaryFormat(bob);

    key = static_cast<unsigned int>(bb.size());
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    s.write(bb.data(), static_cast<std::streamsize>(bb.size()));

    // write vertex array
    key = 0x00000002;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    key = vertices.size();
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    s.write(reinterpret_cast<const char*>(vertices.begin()), vertices.size() * sizeof(Vertex));
    
    // write edge array
    key = 0x00000003;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));

    key = edges.size();
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    s.write(reinterpret_cast<const char*>(edges.begin()), edges.size() * sizeof(Edge));

    // write vertex attribute blob
    key = 0x00000004;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    // length of vertex attribute array is stored in the terminating vertex object
    key = vertices.last().attridx;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    s.write(reinterpret_cast<const char*>(vertexattr.data()), key);
    
    // write edge attribute blob
    key = 0x00000005;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    key = edges.last().attridx;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
    
    s.write(reinterpret_cast<const char*>(edgeattr.data()), key);
    
    // finished
    key = 0x00000000;
    s.write(reinterpret_cast<char*>(&key), sizeof(key));
}

bool GraphData::loadSnapshot(std::istream &s)
{
    unsigned int key, len;
    
    // read signature
    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
    if (key != 0x12121212) return false;

    // read vertices
    while( s.read(reinterpret_cast<char*>(&key), sizeof(key)) )
    {
	switch(key)
	{
	case 0: // termination
	    return true;

	case 1: // graph properties: compare with configured properties
	{
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    ByteBuffer bb1;
	    ByteOutBuffer bob1 (bb1);
	    properties.appendBinaryFormat(bob1);

	    // GraphProperties serialization length doesnt match
	    if (bb1.size() != len) return false;

	    ByteBuffer bb2(len);
	    ByteOutBuffer bob2 (bb2);

	    if (!s.read(reinterpret_cast<char*>(bb2.data()), len)) return false;
	    
	    // GraphProperties serialization doesnt match
	    if (memcmp(bb1.data(), bb2.data(), len) != 0) return false;

	    break;
	}
	case 2: // vertex array
	{
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    std::cerr << "Loading " << len-1 << " vertices." << std::endl;

	    vertices.reserve(len);
	    if (!s.read(reinterpret_cast<char*>(vertices.begin()), len * sizeof(Vertex))) return false;

	    vertices.set_size(len);
	    vertexcount = len;	// TODO: this is wrong, it should be stored in the snapshot
	    break;
	}
	case 3: // edge array
	{
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    std::cerr << "Loading " << len-1 << " edges." << std::endl;

	    edges.reserve(len);
	    if (!s.read(reinterpret_cast<char*>(edges.begin()), len * sizeof(Edge))) return false;

	    edges.set_size(len);
	    break;
	}
	case 4: // vertex attribute array
	{
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    std::cerr << "Loading " << len << " bytes of vertex attributes." << std::endl;

	    vertexattr.alloc(len);
	    if (!s.read(reinterpret_cast<char*>(vertexattr.data()), len)) return false;

	    assert(vertices.last().attridx == len);
	    break;
	}
	case 5: // edge attribute array
	{
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;

	    std::cerr << "Loading " << len << " bytes of edge attributes." << std::endl;

	    edgeattr.alloc(len);
	    if (!s.read(reinterpret_cast<char*>(edgeattr.data()), len)) return false;

	    assert(edges.last().attridx == len);
	    break;
	}
	default:
	    return false;
	}
    }

    // while loop terminated by istream error
    return false;
}

} // namespace VGServer
