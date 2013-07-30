// $Id: GraphProperties.h 85 2006-04-02 20:28:44Z bingmann $

#ifndef VGS_GraphProperties_H
#define VGS_GraphProperties_H

#include "GraphTypes.h"
#include "AttributeProperties.h"

namespace VGServer {

/** GraphProperties is a conglomerate of important unchangeable properties of
 * the graph. It contains if the graph is directed of undirected, the max id of
 * the vertices, the attribute type properties of vertices and edges. This
 * class is loaded once from the application and subsequent changes to it will
 * clear all graph data. \ingroup Public */

class GraphProperties
{
public: //protected:

    /// graph is directed. undirected graphs can be saved more compactly.
    bool	directed;

    /// attribute types of the vertices
    class AttributePropertiesList	vertexattr;

    /// attribute types of the edges
    class AttributePropertiesList	edgeattr;

public:

    /// clears the current config
    void 	clear()
    {
	directed = false;
    
	vertexattr.clear();
	edgeattr.clear();
    }

    /// checks if the graph is directed
    inline bool	isDirected() const
    { return directed; }
    
    /// calculate the lookup-indices and default bit field indices in the
    /// attribute lists.
    void	calcAttributeLookups()
    {
	vertexattr.calcAttributeLookups();
	edgeattr.calcAttributeLookups();
    }

    /// run the GraphPropertiesParser on the given input string which is a
    /// config file's contents.
    bool	parseConfigString(const std::string &s);

    /// concatenate the binary representation of the graph properties to the buffer
    void	appendBinaryFormat(class ByteOutBuffer &bob) const;
};

} // namespace VGServer

#endif // VGS_GraphProperties_H
