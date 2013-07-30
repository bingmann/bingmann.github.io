// $Id: ChangeTimeline.cc 114 2006-04-30 18:01:54Z bingmann $

#include "ChangeTimeline.h"
#include "Changelist.h"

namespace VGServer {

ChangeTimeline::ChangeTimeline(const class GraphData &_graph)
    : graph(_graph)
{
    clstart = clend = new Changelist(graph);
}

ChangeTimeline::~ChangeTimeline()
{
    if (clstart == clend)
    {
	delete clstart;
    }
    else
    {
	delete clstart;
	delete clend;
    }
}

void ChangeTimeline::clear()
{
    if (clstart == clend)
    {
	clstart->clear();
	frames.clear();
    }
    else
    {
	delete clend;
	clstart->clear();
	frames.clear();
	clend = clstart;
    }
}

unsigned int ChangeTimeline::advanceTimeFrame()
{
    if (getFrameCount() == 0)
    {
	// copy the changelist.
	clend = new Changelist(*clstart);
    }

    frames.push_back(ChangeFrame());
    return getFrameCount();
}

bool ChangeTimeline::addVertex(vertexid_t id)
{
    if ( clend->addVertex(id) )
    {
	if (getFrameCount() > 0) frames.back().addVertex(id, clend->getVertexChange(id));
	return true;
    }
    return false;
}

bool ChangeTimeline::setVertexAttr(vertexid_t id, unsigned int attrid, const AnyType &value)
{
    if ( clend->setVertexAttr(id, attrid, value) )
    {
	if (getFrameCount() > 0) frames.back().setVertexAttr(id, clend->getVertexChange(id));
	return true;
    }
    return false;
}

bool ChangeTimeline::delVertex(vertexid_t id)
{
    if ( clend->delVertex(id) )
    {
	if (getFrameCount() > 0) frames.back().delVertex(id);
	return true;
    }
    return false;
}

bool ChangeTimeline::addEdge(vertexid_t src, vertexid_t tgt)
{
    if ( clend->addEdge(src, tgt) )
    {
	if (getFrameCount() > 0) frames.back().addEdge(src, tgt, clend->getEdgeChange(src,tgt));
	return true;
    }
    return false;
}

bool ChangeTimeline::setEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attrid, const AnyType &value)
{
    if ( clend->setEdgeAttr(src, tgt, attrid, value) )
    {
	if (getFrameCount() > 0) frames.back().setEdgeAttr(src, tgt, clend->getEdgeChange(src,tgt));
	return true;
    }
    return false;
}

bool ChangeTimeline::delEdge(vertexid_t src, vertexid_t tgt)
{
    if ( clend->delEdge(src, tgt) )
    {
	if (getFrameCount() > 0) frames.back().delEdge(src, tgt);
	return true;
    }
    return false;
}

// *** ChangeFrame

void ChangeFrame::addVertex(vertexid_t id, const AttributeVertexTinyBlob &b)
{
    chgoplist.push_back( ChangeEntry(CHG_ADDVERTEX, id, b) );
}

void ChangeFrame::setVertexAttr(vertexid_t id, const AttributeVertexTinyBlob &b)
{
    chgoplist.push_back( ChangeEntry(CHG_SETVERTEX, id, b) );
}

void ChangeFrame::delVertex(vertexid_t id)
{
    chgoplist.push_back( ChangeEntry(CHG_DELVERTEX, id, 0) );
}

void ChangeFrame::addEdge(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &b)
{
    chgoplist.push_back( ChangeEntry(CHG_ADDEDGE, src, tgt, b) );
}

void ChangeFrame::setEdgeAttr(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &b)
{
    chgoplist.push_back( ChangeEntry(CHG_SETEDGE, src, tgt, b) );
}

void ChangeFrame::delEdge(vertexid_t src, vertexid_t tgt)
{
    chgoplist.push_back( ChangeEntry(CHG_DELEDGE, src, tgt) );
}


} // namespace VGServer
