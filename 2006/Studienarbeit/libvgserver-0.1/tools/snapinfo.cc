// $Id: snapinfo.cc 204 2006-06-18 14:45:04Z bingmann $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "GraphLoaderKFile.h"
#include "ChangeTimeline.h"
#include "RTree.h"

#include <stdlib.h>
#include <iostream>
#include <numeric>

// load a .ksnap file from std::cin and show size information about it.

using namespace VGServer;

int main(int argc, char *argv[])
{
    // these GraphProperties might not be the final ones.

    GraphProperties gp;
    gp.directed = false;

    gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
    gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
    gp.vertexattr.push_back(AttributeProperties("level", AnyType(int(0)) ));

    gp.vertexattr[0].varying = true;
    gp.vertexattr[1].varying = true;

    gp.edgeattr.push_back(AttributeProperties("distance", AnyType(int(0)) ));
    gp.edgeattr.push_back(AttributeProperties("direction", AnyType(char(0)) ));
    gp.edgeattr.push_back(AttributeProperties("speed", AnyType(char(0)) ));

    gp.calcAttributeLookups();

    // load container snapshot
    GraphContainer gc (gp);

    if (!gc.loadSnapshot(std::cin)) {
	std::cerr << "Could not load snapshot\n";
	return 0;
    }

    std::cout << "number of vertices: " << gc.getVertexCount() << "\n";
    std::cout << "number of edges: " << gc.getEdgeCount() << "\n";

    std::cout << "bytes for vertex objects: " << gc.getVertexCount() * sizeof(GraphData::Vertex) << "\n";
    std::cout << "bytes for edge objects: " << gc.getEdgeCount() * sizeof(GraphData::Edge) << "\n";
    std::cout << "bytes for both object arrays: " << gc.getVertexCount() * sizeof(GraphData::Vertex) + gc.getEdgeCount() * sizeof(GraphData::Edge) << "\n";

    std::cout << "bytes for vertex attributes: " << gc.getVertexAttrBytes() << "\n";
    std::cout << "bytes for edge attributes: " << gc.getEdgeAttrBytes() << "\n";
    std::cout << "bytes for both attributes: " << gc.getVertexAttrBytes() + gc.getEdgeAttrBytes() << "\n";

    std::cout << "rtrees: " << gc.getRTreeNum() << "\n";

    GraphContainer::RTreeStats_t rtree_st;
    gc.getRTreeStats(rtree_st);

    GraphContainer::RTreeLevelStats_t rtree_totalst;
    rtree_totalst = std::accumulate(rtree_st.level.begin(), rtree_st.level.end(), rtree_totalst);

    std::cout << "rtree bytes: " << rtree_totalst.bytes << "\n";

    std::cout << "total size: "
	      << gc.getVertexCount() * sizeof(GraphData::Vertex) + gc.getEdgeCount() * sizeof(GraphData::Edge)
	       + gc.getVertexAttrBytes() + gc.getEdgeAttrBytes()
	       + rtree_totalst.bytes
	      << "\n";

    return 0;
}
