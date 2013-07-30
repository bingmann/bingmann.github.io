// $Id: GraphLoader.cc 244 2006-07-07 09:11:51Z schultes $

#include "GraphLoader.h"
#include "GraphData.h"

#include "AttributeBlob_impl.h"

#include <stdlib.h>

namespace VGServer {

GraphLoader::GraphLoader(const class GraphProperties &gp)
    : GraphData(gp)
{
    vertex_minnum = 0;
    vertex_aidx = vertex_aidxnext = 0;

    edge_minsrc = edge_mintgt = 0;
    edge_eidx = 0;
    edge_aidx = edge_aidxnext = 0;

    // clears the termination nodes, which are added by GraphData's constructor.
    vertices.clear();
    edges.clear();
}

void GraphLoader::reserve(unsigned int vertexnum, unsigned int edgenum,
			  unsigned int vertexattrsize, unsigned int edgeattrsize)
{
    vertices.reserve(vertexnum);
    edges.reserve(edgenum);

    vertexattr.alloc(vertexattrsize);
    edgeattr.alloc(edgeattrsize);
}

class GraphData& GraphLoader::finish()
{
    if (vertex_minnum != VERTEX_INVALID) terminate();

    return *this;
}

void GraphLoader::terminate()
{
    if (vertex_minnum > 0)
	finishVertexAttrSequence();
    
    // adds termination vertex of the vertices[].attridx ascending sequence and
    // fill in edgeidx for vertices without edges.
    {
	GraphData::Vertex& vend = vertices.new_back();
	vend.attridx = vertex_aidxnext;

	for (unsigned int v = edge_minsrc; v < vertices.size(); v++)
	{
	    vertices[v].edgeidx = edge_eidx;
	}
    }

    if (edges.size() > 0)
	finishEdgeAttrSequence();
    
    // add termination edge object for edgeidx and attridx
    {
	GraphData::Edge& eend = edges.new_back();
	eend.attridx = edge_aidxnext;
    }

    vertex_minnum = VERTEX_INVALID;
    edge_minsrc = edge_mintgt = VERTEX_INVALID;
}

void GraphLoader::addVertex(vertexid_t vid)
{
    if (vid < vertex_minnum) throw OrderException();

    if (vertex_minnum > 0)
	finishVertexAttrSequence();

    // grow vertex buffer, this code will be deleted as the TpArray auto-grows.
#ifndef NDEBUG
    unsigned int oldcap = vertices.capacity();
    vertices.reserve(vid+2);
    if (oldcap != vertices.capacity()) 
	fprintf(stderr,"New vertex capacity: %d\n", vertices.capacity());
#endif

    // create nodes with filled in attridx:
    // set all vertex.attridx from the last added+1 vertex up to this to the
    // current pointer
    for (unsigned int v = vertex_minnum; v <= vid; v++)
    {
	// figure out if we have to add space for varying attributes?
	// no we dont: varying attribute space may not exist
	GraphData::Vertex &vnew = vertices.new_back();
	assert(vertices.size() == v+1);

	vnew.attridx = vertex_aidxnext;
    }

    vertexcount++;

    vertex_minnum = vid+1;

    // position of the default bitfield
    vertex_aidx = vertex_aidxnext;

    // clear default bits
    vertexattr.zero(vertex_aidx, properties.vertexattr.defbytes);

    // advance the attribute write pointer the number of default bits.
    vertex_aidxnext = vertex_aidx + properties.vertexattr.defbytes;

    // reset attribute loading sequence
    vertex_lastid = 0;
}

void GraphLoader::setVertexAttr(vertexid_t vid, unsigned int attrid, const class AnyType &value)
{
    if (vid+1 < vertex_minnum) throw OrderException();
    if (attrid < vertex_lastid) throw OrderException();

    const AttributePropertiesList &attrlist = properties.vertexattr;

    if (attrid >= attrlist.size()) throw AttributeIdException();

    if (vertex_minnum < vid+1)
	addVertex(vid); // add missing vertex entries

    // now vertex_aidx is the right pointer

    // fill in skipped varying attributes
    for(unsigned int ai = vertex_lastid; ai < attrid; ai++)
    {
	if (attrlist[ai].getType() == ATTRTYPE_BOOL)
	{
	    // put the default value of the bool attribute into the default bitfield
	    vertexattr.putBool(vertex_aidx, attrlist[ai].defbitnum, attrlist[ai].getInteger() != 0);
	}
	else if (attrlist[ai].varying)
	{
	    // incase we skip over a varying attribute, we have to leave
	    // space in the attribute value array

	    // zeros the value of a varying attribute. writes the default value
	    // of a varying attribute.
	    vertexattr.putAnyType(vertex_aidxnext, attrlist[ai]);

	    vertex_aidxnext += attrlist[ai].getValueLength();
	}
	else
	{
	    // for attributes with default values, set the default bit.
	    vertexattr.putBool(vertex_aidx, attrlist[ai].defbitnum, true);
	}
    }

    // now vertex_aidxnext points to the place to insert this value
    if (value.getType() == attrlist[attrid].getType())
    {
	if (attrlist[attrid].getType() == ATTRTYPE_BOOL)
	{
	    // put the value in the default bitfield
	    vertexattr.putBool(vertex_aidx, attrlist[attrid].defbitnum, value.getInteger() != 0);
	}
	else
	{
	    // save directly as the value parameter has the right type
	    vertexattr.putAnyType(vertex_aidxnext, value);
	    vertex_aidxnext += value.getValueLength();
	}
    }
    else
    {
	AnyType tv = value; // convert value
	tv.convertType(attrlist[attrid].getType());

	if (attrlist[attrid].getType() == ATTRTYPE_BOOL)
	{
	    // put the value in the default bitfield
	    vertexattr.putBool(vertex_aidx, attrlist[attrid].defbitnum, tv.getInteger() != 0);
	}
	else
	{
	    vertexattr.putAnyType(vertex_aidxnext, tv);
	    vertex_aidxnext += tv.getValueLength();
	}
    }

    if (not attrlist[attrid].varying && attrlist[attrid].getType() != ATTRTYPE_BOOL)
    {
	// clear the default bit.
	vertexattr.putBool(vertex_aidx, attrlist[attrid].defbitnum, false);
    }

    vertex_lastid = attrid+1;
}

void GraphLoader::finishVertexAttrSequence()
{
    const AttributePropertiesList &attrlist = properties.vertexattr;
    
    for(unsigned int ai = vertex_lastid; ai < attrlist.size(); ai++)
    {
	if (attrlist[ai].getType() == ATTRTYPE_BOOL)
	{
	    // put the default value of the bool attribute into the default bitfield
	    vertexattr.putBool(vertex_aidx, attrlist[ai].defbitnum, attrlist[ai].getInteger() != 0);
	}
	else if (attrlist[ai].varying)
	{
	    // incase we skip over a varying attribute, we have to leave
	    // space in the attribute value array

	    // zeros the value of a varying attribute. writes the default value
	    // of a varying attribute.
	    vertexattr.putAnyType(vertex_aidxnext, attrlist[ai]);

	    vertex_aidxnext += attrlist[ai].getValueLength();
	}
	else
	{
	    // for attributes with default values, set the default bit.
	    vertexattr.putBool(vertex_aidx, attrlist[ai].defbitnum, true);
	}
    }
   
    vertex_lastid = attrlist.size()+1;
}

void GraphLoader::addEdge(vertexid_t src, vertexid_t tgt)
{
    if ( not ( (src == edge_minsrc-1 and tgt >= edge_mintgt) or (src >= edge_minsrc) ) )
	throw OrderException();

    // check vertex buffer size: cannot add edges to unloaded vertex ids
    // (target id doesnt matter, it can be larger)
    if (src >= vertices.size())
	throw DataLoadedException();

    // this code should also be delete.
#ifndef NDEBUG
    unsigned int oldcap = edges.capacity();
    edges.reserve(edges.size()+1);
    if (oldcap != edges.capacity()) 
	fprintf(stderr,"New edge capacity: %d\n", edges.capacity());
#endif

    // fill in remaining varying/default attribute values of last edge
    if (edges.size() > 0) finishEdgeAttrSequence();
    
    // allocate the edge object in the edge adjacency array
    GraphData::Edge &enew = edges.new_back();
    enew.target = tgt;

    // the added object is right at the end.
    assert( edge_eidx+1 == edges.size() );
    // see assertion. edge_eidx = edges.size()-1;

    // fill in edgeidx for skipped and this source vertices:
    // set all vertex.edgeidx from the last source vertex+1 up to this to the
    // current pointer
    for (unsigned int v = edge_minsrc; v <= src; v++)
    {
	vertices[v].edgeidx = edge_eidx;
    }

    edge_minsrc = src+1;
    edge_mintgt = tgt+1;

    // position of the default bitfield
    edge_aidx = edge_aidxnext;
    edges[edge_eidx].attridx = edge_aidx;

    edge_eidx++; // eidx points to the next index to be used.

    // clear default bits
    edgeattr.zero(edge_aidx, properties.edgeattr.defbytes);

    // advance the attribute write pointer the number of default bits.
    edge_aidxnext = edge_aidx + properties.edgeattr.defbytes;

    // reset attribute loading sequence
    edge_lastid = 0;
}

void GraphLoader::setEdgeAttr(vertexid_t src, vertexid_t tgt,
			      unsigned int attrid, const class AnyType &value)
{
    if ( not ( (src == edge_minsrc-1 and tgt+1 >= edge_mintgt) or (src >= edge_minsrc) ) )
	throw OrderException();

    if (attrid < edge_lastid) throw OrderException();

    const AttributePropertiesList &attrlist = properties.edgeattr;

    if (attrid >= attrlist.size()) throw AttributeIdException();

    if (edge_minsrc != src+1 or edge_mintgt != tgt+1)
	addEdge(src,tgt); // advance to the right edge

    // now edge_aidx is the right pointer

    // fill in skipped varying attributes
    for(unsigned int ai = edge_lastid; ai < attrid; ai++)
    {
	if (attrlist[ai].getType() == ATTRTYPE_BOOL)
	{
	    // put the default value of the bool attribute into the default bitfield
	    edgeattr.putBool(edge_aidx, attrlist[ai].defbitnum, attrlist[ai].getInteger() != 0);
	}
	else if (attrlist[ai].varying)
	{
	    // incase we skip over a varying attribute, we have to leave
	    // space in the attribute value array

	    // write "default" zero of the varying attribute.
	    edgeattr.putAnyType(edge_aidxnext, attrlist[ai]);

	    edge_aidxnext += attrlist[ai].getValueLength();
	}
	else
	{
	    // for attributes with default values, set the default bit.
	    edgeattr.putBool(edge_aidx, attrlist[ai].defbitnum, true);
	}
    }

    // now edge_aidxnext points to the place to insert this value
    if (value.getType() == attrlist[attrid].getType())
    {
	if (attrlist[attrid].getType() == ATTRTYPE_BOOL)
	{
	    // put the value in the default bitfield
	    edgeattr.putBool(edge_aidx, attrlist[attrid].defbitnum, value.getInteger() != 0);
	}
	else
	{
	    // save directly as the value parameter has the right type
	    edgeattr.putAnyType(edge_aidxnext, value);
	    edge_aidxnext += value.getValueLength();
	}
    }
    else
    {
	AnyType tv = value; // convert value
	tv.convertType(attrlist[attrid].getType());

	if (attrlist[attrid].getType() == ATTRTYPE_BOOL)
	{
	    // put the value in the default bitfield
	    edgeattr.putBool(edge_aidx, attrlist[attrid].defbitnum, tv.getInteger() != 0);
	}
	else
	{
	    edgeattr.putAnyType(edge_aidxnext, tv);
	    edge_aidxnext += tv.getValueLength();
	}
    }

    if (not attrlist[attrid].varying && attrlist[attrid].getType() != ATTRTYPE_BOOL)
    {
	// clear the default bit.
	edgeattr.putBool(edge_aidx, attrlist[attrid].defbitnum, false);
    }

    edge_lastid = attrid+1;
}

void GraphLoader::finishEdgeAttrSequence()
{
    const AttributePropertiesList &attrlist = properties.edgeattr;
    
    for(unsigned int ai = edge_lastid; ai < attrlist.size(); ai++)
    {
	if (attrlist[ai].getType() == ATTRTYPE_BOOL)
	{
	    // put the default value of the bool attribute into the default bitfield
	    edgeattr.putBool(edge_aidx, attrlist[ai].defbitnum, attrlist[ai].getInteger() != 0);
	}
	else if (attrlist[ai].varying)
	{
	    // incase we skip over a varying attribute, we have to leave
	    // space in the attribute value array

	    // write "default" zero of the varying attribute.
	    edgeattr.putAnyType(edge_aidxnext, attrlist[ai]);

	    edge_aidxnext += attrlist[ai].getValueLength();
	}
	else
	{
	    // for attributes with default values, set the default bit.
	    edgeattr.putBool(edge_aidx, attrlist[ai].defbitnum, true);
	}
    }
    edge_lastid = attrlist.size()+1;
}

} // namespace VGServer
