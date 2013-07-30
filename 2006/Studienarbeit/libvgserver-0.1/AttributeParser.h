// $Id: AttributeParser.h 241 2006-07-05 07:29:52Z bingmann $

#ifndef VGS_AttributeParser_H
#define VGS_AttributeParser_H

#include <string>
#include "AnyType.h"
#include "AttributeProperties.h"

#include "AttributeBlob.h"

namespace VGServer {

/** ParseNode is the abstract interface for filter or selection nodes
 * constructed by the AttributeParser. It offers abstract virtual methods to
 * calculate the value of a selected Attribute from the attribute chain of a
 * vertex or an edge. */

class ParseNode
{
protected:
    /// usual construction
    inline ParseNode()
    { }

    /// disable copy construction
    ParseNode(const ParseNode &pn);
    
public:
    /// virtual destructor so derived classes can deallocate their children
    /// nodes.
    virtual ~ParseNode()
    { }

    /// function to retrieve the selected (calculated) value based on a vertex
    /// or edge attribute blob.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t voe, unsigned int vid1, unsigned int vid2) const = 0;

    /// function to check if the subtree evalulates to a constant expression,
    /// if dest == NULL then do a static check if the node is always a
    /// constant (ignoring subnodes), if dest != NULL try to calculate the
    /// constant value and type recursively, thus the return value can be true
    /// for a non-constant tree node. The calculated value will be used as the
    /// default value of the attribute.
    virtual bool getConstVal(AnyType *dest) const = 0;

    /// more a debug function which returns the parsed selection specification
    /// as a string.
    virtual std::string toString() const = 0;
};

/** AttributeSelectorList holds a vector of AttributeSelectors representing the
 * array of return values of a selection query. It is derived from
 * AttributePropertiesList in order to inherit the Attribute Binary Description
 * Format functions. */

class AttributeSelectorList : private std::vector<const class ParseNode*>,
			      public AttributePropertiesList
{
private:
    /// parent class which is the vector containing the selection tree root nodes.
    typedef std::vector<const class ParseNode*> sellist;

    /// parent class holding the attribute property objects
    typedef AttributePropertiesList attrlist;

public:
    /// called to free all the selector trees
    ~AttributeSelectorList()
    {
	clear();
    }

    /// clears the selector list
    void clear() {
	attrlist::clear();
	for(sellist::const_iterator i = sellist::begin(); i != sellist::end(); i++) {
	    delete *i;
	}
	sellist::clear();
    }
    
    /// do the * selection: add all attributes from the AttributePropertiesList
    void 	selectStar(const AttributePropertiesList &otherlist, vertex_or_edge_t voe);

    /// add a new attribute via AttributeSelector tree. the type of the tree is
    /// determined via the getConstVal function.
    void 	addAttrsel(const class ParseNode *attrsel);

    /// uses the spirit parser to build a parse tree and fills this structure
    /// with the resulting selector trees. Returns true if parsing was
    /// successful, throws Exceptions for various syntactic and semantic
    /// errors.
    bool	parseString(const std::string &input, const class GraphProperties &gp, vertex_or_edge_t voe);

    /// more a debug function which returns the parsed selection specification
    /// as a string.
    std::string toString() const;

    /// ambigious call because AttributePropertiesList is-as vector. But their
    /// size must be the same anyway.
    size_t size() const {
	assert(sellist::size() == attrlist::size());
	return sellist::size();
    }

    /// operator[] is an ambigious call.
    inline const class ParseNode* getSelector(unsigned int i) const
    { return sellist::operator[](i); }

    /// operator[] is an ambigious call.
    inline const class AttributeProperties& getProperties(unsigned int i) const
    { return attrlist::operator[](i); }

    /// create a filled AttributeVertexTinyBlob containing the vertex values
    /// extracted by this selector list. Return the length of the resulting
    /// attribute chain.
    inline unsigned int processVertexAttributeBlob(AttributeVertexTinyBlob &attrblob,
						   const class GraphContainer &gc,
						   const class Changelist &cl, unsigned int vid) const
    {
	unsigned int p = defbytes;
	attrblob.zero(0, p);
	
	for(unsigned int ai = 0; ai < size(); ai++)
	{
	    // Calculate the value of the attribute selector
	    AnyType attrval(ATTRTYPE_INVALID);

	    getSelector(ai)->getValue(attrval, gc, cl, VE_VERTEX, vid, 0);

	    // bools are in the default bitfield
	    if (getProperties(ai).getType() == ATTRTYPE_BOOL) {
		attrblob.putBool(0, ai, attrval.getInteger() != 0);
	    }
	    // check if it's the default value.
	    else if (attrval == getProperties(ai)) {
		// set default bit
		attrblob.putBool(0, ai, true);
	    }
	    else {
		// otherwise append the value.
		attrblob.putAnyType(p, attrval);
		p += attrval.getValueLength();
	    }
	}

	return p;
    }

    /// create a filled AttributeVertexTinyBlob containing the edge values
    /// extracted by this selector list. Return the length of the resulting
    /// attribute chain.
    inline unsigned int processEdgeAttributeBlob(AttributeVertexTinyBlob &attrblob,
						 const class GraphContainer &gc,
						 const class Changelist &cl, unsigned int esrc, unsigned etgt) const
    {
	unsigned int p = defbytes;
	attrblob.zero(0, p);
	
	for(unsigned int ai = 0; ai < size(); ai++)
	{
	    // Calculate the value of the attribute selector
	    AnyType attrval(ATTRTYPE_INVALID);

	    getSelector(ai)->getValue(attrval, gc, cl, VE_EDGE, esrc, etgt);

	    // bools are in the default bitfield
	    if (getProperties(ai).getType() == ATTRTYPE_BOOL) {
		attrblob.putBool(0, ai, attrval.getInteger() != 0);
	    }
	    // check if it's the default value.
	    else if (attrval == getProperties(ai)) {
		// set default bit
		attrblob.putBool(0, ai, true);
	    }
	    else {
		// otherwise append the value.
		attrblob.putAnyType(p, attrval);
		p += attrval.getValueLength();
	    }
	}

	return p;
    }

};

/** FilterRoot holds the root node of either an edge filter or a vertex
 * filter. It is used to hold the result of a parsed filter expression. */

class FilterRoot
{
private:
    /// the filter subtree
    const class ParseNode	*filter;

    /// whether the filter is a vertex or edge.
    vertex_or_edge_t	vertexoredge;

public:
    FilterRoot()
	: filter(NULL)
    {
    }

    /// called to free all the filter trees
    ~FilterRoot()
    {
	if (filter)
	    delete filter;
    }

    /// uses the spirit parser to build a parse tree and fills this structure
    /// with the resulting filter tree. Returns true if parsing was successful,
    /// throws Exceptions for various syntactic and semantic errors.
    bool	parseString(const std::string &input, const class GraphProperties &gp);

    /// more a debug function which returns the parsed selection specification
    /// as a string.
    std::string	toString() const;

    /// returns true if the filter is the null-filter.
    bool	isEmpty() const
    { return (filter == NULL); }

    /// returns the type of filter: vertex or edge filter
    vertex_or_edge_t	getType() const
    { return vertexoredge; }

    /// evaluate the filter for a vertex
    inline bool eval_vertex(const class GraphContainer &gc, const class Changelist &cl,
			    unsigned int vid) const
    {
	AnyType at(ATTRTYPE_INVALID);

	assert(filter);
	filter->getValue(at, gc, cl, VE_VERTEX, vid, 0);
	return (at.getInteger() != 0);
    }

    /// evaluate the filter for an edge
    inline bool eval_edge(const class GraphContainer &gc, const class Changelist &cl,
			  unsigned int vidsrc, unsigned int vidtgt) const
    {
	AnyType at(ATTRTYPE_INVALID);

	assert(filter);
	filter->getValue(at, gc, cl, VE_EDGE, vidsrc, vidtgt);
	return (at.getInteger() != 0);
    }
};

/** \ingroup Exception
 * AttributeParseException is an exception class thrown when the parser
 * recognizes a syntax error. */

class AttributeParseException : public GraphException
{
public:
    inline AttributeParseException(const std::string &s) throw()
	: GraphException(s)
    { }
};


} // namespace VGServer

#endif // VGS_AttributeParser_H

