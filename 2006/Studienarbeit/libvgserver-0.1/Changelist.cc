// $Id: Changelist.cc 243 2006-07-06 07:57:25Z bingmann $

#include "Changelist.h"
#include "AttributeBlob_impl.h"

#include <algorithm>
#include <iostream>

namespace VGServer {

Changelist::Changelist(const class GraphData &_graph)
    : graph(_graph)
{
}

void Changelist::clear()
{
    vertexchglist.clear();
    vertexaddlist.clear();

    edgechglist.clear();
    edgeaddlist.clear();
}

bool Changelist::addVertex(vertexid_t id)
{
    vertexchglist_t::iterator v = vertexchglist.find(id);
    if (v != vertexchglist.end())
    {
	// check that the modification was a delete.
	if (not v->second.empty()) return false;

	// replace modification with the vertex's original data or blank
	// attribute values
	AttributeVertexTinyBlob &ab = vertexchglist[id] = graph.getVertexAttrBlob(id);

	if (ab.empty()) {
	    // fill in default data
	    vertexchglist[id] = graph.properties.vertexattr.createBlankAttributeBlob<AttributeVertexTinyBlob>();
	}
    }
    else
    {
	// check that the vertex doesnt exist in the global graph
	if (graph.existVertex(id)) return false;

	// put default vertex data into the map
	vertexchglist[id] = graph.properties.vertexattr.createBlankAttributeBlob<AttributeVertexTinyBlob>();
    }

    assert( vertexaddlist.find(id) == vertexaddlist.end() );
    vertexaddlist.insert(id);

    return true;
}

bool Changelist::addVertex(vertexid_t id, const AttributeVertexTinyBlob &vb)
{
    vertexchglist_t::iterator v = vertexchglist.find(id);
    if (v != vertexchglist.end())
    {
	// check that the modification was a delete.
	if (not v->second.empty()) return false;

	// replace modification with the vertex's new data blob
	vertexchglist[id] = vb;
    }
    else
    {
	// check that the vertex doesnt exist in the global graph
	if (graph.existVertex(id)) return false;

	// put default vertex data into the map
	vertexchglist[id] = vb;
    }

    assert( vertexaddlist.find(id) == vertexaddlist.end() );
    vertexaddlist.insert(id);

    return true;
}

bool Changelist::setVertexAttr(vertexid_t id, unsigned int attrid, const AnyType &value)
{
    vertexchglist_t::iterator v = vertexchglist.find(id);
    if (v != vertexchglist.end())
    {
	// check that the modification was not a delete.
	if (v->second.empty()) return false;

	// add modification to the blob in the map
	v->second.putAttrChainValue(graph.properties.vertexattr, attrid, value);
    }
    else
    {
	// new modification: get all attribute values from global graph
	AttributeVertexTinyBlob ab = graph.getVertexAttrBlob(id);

	if (ab.empty()) return false; // vertex doesnt exist in global graph

	// make initial modification
	ab.putAttrChainValue(graph.properties.vertexattr, attrid, value);

	// save this afterwards: putAttrChainValue might have thrown an exception
	vertexchglist[id] = ab;
    }
    return true;
}

bool Changelist::setVertexAttr(vertexid_t id, const AttributeVertexTinyBlob &vb)
{
    vertexchglist_t::iterator v = vertexchglist.find(id);
    if (v != vertexchglist.end())
    {
	// check that the modification was not a delete.
	if (v->second.empty()) return false;

	// put modification to the blob in the map
	v->second = vb;
    }
    else
    {
	// check that the vertex exists in the global graph
	if (not graph.existVertex(id)) return false;

	// put change into the map
	vertexchglist[id] = vb;
    }
    return true;
}

bool Changelist::delVertex(vertexid_t id)
{
    vertexchglist_t::iterator v = vertexchglist.find(id);
    if (v != vertexchglist.end())
    {
	// check that the modification was not a delete.
	if (v->second.empty()) return false;

	// replace modification with a deleted vertex
	v->second = AttributeVertexTinyBlob();

	assert( vertexaddlist.find(id) != vertexaddlist.end() );
	vertexaddlist.erase(id);
    }
    else
    {
	// check that the vertex exists in the global graph
	if (not graph.existVertex(id)) return false;

	// add delete entry to map
	vertexchglist[id] = AttributeVertexTinyBlob();
    }
    return true;
}

bool Changelist::addEdge(vertexid_t src, vertexid_t tgt)
{
    edgechglist_t::iterator e = find_edgechglist(src,tgt);

    if (e != edgechglist.end())
    {
	// check that the modification was a delete.
	if (not e->second.empty()) return false;

	// replace modification with the edge's original data or blank
	// attribute values
	AttributeEdgeTinyBlob &ab = e->second = graph.getEdgeAttrBlob(src,tgt);

	if (ab.empty()) {
	    // fill in default data
	    e->second = graph.properties.edgeattr.createBlankAttributeBlob<AttributeEdgeTinyBlob>();
	}
    }
    else
    {
	// check that the vertex doesnt exist in the global graph
	if (graph.existEdge(src,tgt)) return false;

	// put default edge data into the map
	edgechglist.insert( edgechglist_t::value_type(vertexidpair_t(src,tgt),
						      graph.properties.edgeattr.createBlankAttributeBlob<AttributeEdgeTinyBlob>()) );
    }

    assert( edgeaddlist.find( vertexidpair_t(src,tgt) ) == edgeaddlist.end() );
    edgeaddlist.insert( vertexidpair_t(src,tgt) );

    return true;
}

bool Changelist::addEdge(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &eb)
{
    edgechglist_t::iterator e = find_edgechglist(src,tgt);

    if (e != edgechglist.end())
    {
	// check that the modification was a delete.
	if (not e->second.empty()) return false;

	// replace modification with the edge's new data blob
	e->second = eb;
    }
    else
    {
	// check that the vertex doesnt exist in the global graph
	if (graph.existEdge(src,tgt)) return false;

	// put default edge data into the map
	edgechglist.insert( edgechglist_t::value_type(vertexidpair_t(src,tgt), eb) );
    }

    assert( edgeaddlist.find( vertexidpair_t(src,tgt) ) == edgeaddlist.end() );
    edgeaddlist.insert( vertexidpair_t(src,tgt) );

    return true;
}

bool Changelist::setEdgeAttr(vertexid_t src, vertexid_t tgt, unsigned int attrid, const AnyType &value)
{
    edgechglist_t::iterator e = find_edgechglist(src,tgt);
    
    if (e != edgechglist.end())
    {
	// check that the modification was not a delete.
	if (e->second.empty()) return false;

	// add modification to the blob in the map
	e->second.putAttrChainValue(graph.properties.edgeattr, attrid, value);
    }
    else
    {
	// new modification: get all attribute values from global graph
	AttributeEdgeTinyBlob ab = graph.getEdgeAttrBlob(src,tgt);

	if (ab.empty()) return false; // edge doesnt exist in global graph,
				      // call addEdge first.

	// make initial modification
	ab.putAttrChainValue(graph.properties.edgeattr, attrid, value);

	// save this afterwards: putAttrChainValue might have thrown an exception
	edgechglist.insert( edgechglist_t::value_type(vertexidpair_t(src,tgt), ab) );
    }
    return true;
}

bool Changelist::setEdgeAttr(vertexid_t src, vertexid_t tgt, const AttributeEdgeTinyBlob &eb)
{
    edgechglist_t::iterator e = find_edgechglist(src,tgt);
    
    if (e != edgechglist.end())
    {
	// check that the modification was not a delete.
	if (e->second.empty()) return false;

	// add modification to the blob in the map
	e->second = eb;
    }
    else
    {
	// check that the vertex exists in the global graph
	if (not graph.existEdge(src,tgt)) return false;

	// save new modification: put all attribute values 
	edgechglist.insert( edgechglist_t::value_type(vertexidpair_t(src,tgt), eb) );
    }
    return true;
}

bool Changelist::delEdge(vertexid_t src, vertexid_t tgt)
{
    edgechglist_t::iterator e = find_edgechglist(src,tgt);
    
    if (e != edgechglist.end())
    {
	// check that the modification was not a delete.
	if (e->second.empty()) return false;

	// replace modification with a deleted edge
	e->second = AttributeEdgeTinyBlob();

	assert( edgeaddlist.find( vertexidpair_t(src,tgt) ) != edgeaddlist.end() );
	edgeaddlist.erase( vertexidpair_t(src,tgt) );
    }
    else
    {
	// check that the edge exists in the global graph
	if (not graph.existEdge(src,tgt)) return false;

	// add delete entry to map
	edgechglist.insert( edgechglist_t::value_type(vertexidpair_t(src,tgt),
						  AttributeEdgeTinyBlob()) );
    }
    return true;
}

std::vector<Changelist::edgeblobpair_t> Changelist::getEdgeListChange(vertexid_t src) const
{
    std::vector<edgeblobpair_t> ev;

    unsigned int ec = edgechglist.count(vertexidpair_t(src,0));
    if (ec == 0) return ev;

    edgechglist_t::const_iterator e = edgechglist.find(vertexidpair_t(src, 0));

    while( e != edgechglist.end() )
    {
	if (e->first.first == src)
	{
	    ev.push_back( edgeblobpair_t(e->first.second, &e->second) );

	    if (--ec == 0) break;
	}

	++e;
    }

    std::sort( ev.begin(), ev.end() );

    return ev;
}

} // namespace VGServer
