// $Id: rtreefig.cc 179 2006-06-07 06:04:17Z bingmann $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "GraphLoaderKFile.h"
#include "RTree.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>

// load the .k file from std::cin and write the r-tree fig file to std::cout

using namespace VGServer;

typedef GraphContainer::RTree_t RTree_t;

const int vertexattrid_xaxis = 0;
const int vertexattrid_yaxis = 1;

/// callback context class used to get a MBR from a data object
struct RTreeDataCallback
{
    GraphData &gd;

    RTreeDataCallback(GraphData &_gd)
	: gd(_gd)
    { }
	
    inline const class RTree_t::Rect getMBR(const GraphContainer::RTreeData &d) const
    {
	int x1 = gd.getVertexAttr(d.src, vertexattrid_xaxis).getInteger();
	int y1 = gd.getVertexAttr(d.src, vertexattrid_yaxis).getInteger();

	int x2 = gd.getVertexAttr(d.tgt, vertexattrid_xaxis).getInteger();
	int y2 = gd.getVertexAttr(d.tgt, vertexattrid_yaxis).getInteger();

	return RTree_t::Rect(std::min(x1,x2), std::min(-y1,-y2),
			     std::max(x1,x2), std::max(-y1,-y2));
    }
};

typedef RTree_t::Tree<struct GraphContainer::RTreeData, struct RTreeDataCallback> GraphRTree;

inline double timestamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return double(tp.tv_sec) + tp.tv_usec / 1000000.;
}

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

#if 0
    // load k file

    GraphLoaderKFile loader(gp);

    if (!loader.load(std::cin)) return 0;

    GraphData &gd = loader.finish();
#else
    // load data snapshot

    GraphData gd (gp);

    if (!gd.loadSnapshot(std::cin)) return 0;
#endif

    for(unsigned int reinsertnum = 0; reinsertnum <= 20; reinsertnum++)
    {
    // build rtree over all levels

    RTreeDataCallback callback(gd);

    GraphRTree tree (&callback);
    tree.reinsertnum = reinsertnum;
    double ts1 = timestamp();

    std::cerr << "Reinsertnum: " << tree.reinsertnum << "\n";
    std::cerr << "Rebuilding R-Tree: 0%";
    std::cerr.flush();

    unsigned int dotstepmax = (gd.edges.size()-1) / 60;
    unsigned int dotstep = 0;
    unsigned int percentstepmax = 6;
    unsigned int percentstep = 0;

    for(vertexid_t vi = 0; vi < gd.vertices.size()-1; vi++)
    {
	if (not gd.existVertex(vi)) continue;

	//int x1 = gd.getVertexAttr(vi, 0).getInteger();
	//int y1 = gd.getVertexAttr(vi, 1).getInteger();

	unsigned int endedgeidx = gd.vertices[vi+1].edgeidx;

	for(unsigned int ei = gd.vertices[vi].edgeidx; ei < endedgeidx; ei++)
	{
	    vertexid_t tvi = gd.edges[ei].target;
	    if (tvi == VERTEX_INVALID) break;
	    if (not gd.existVertex(tvi)) continue;

	    //int x2 = gd.getVertexAttr(tvi, 0).getInteger();
	    //int y2 = gd.getVertexAttr(tvi, 1).getInteger();

	    // unsigned int speed = gd.edges[ei].getAttr(2, gd).getInteger();
	    // if (speed != 11) continue;

	    tree.insertRect(GraphRTree::DataType(vi, tvi));

	    //assert(tree.testTree());

	    // dumb and funny progress status
	    if (++dotstep >= dotstepmax) {
		std::cerr << ".";
		dotstep = 0;

		if (++percentstep > percentstepmax)
		{
		    std::cerr << int(double(ei) / double(gd.edges.size()-1) * 100.0) << "%";
		    percentstep = 0;
		}

		std::cerr.flush();
	    }
	}
    }

    std::cerr << "100% done\n";

    double ts2 = timestamp();

    assert(tree.testTree());

    std::cerr << "Build Time: " << (ts2-ts1) << "sec\n";
    std::cerr << "Overlap: " << tree.calcOverlap() << "\n";
    std::cerr << "Waste: " << tree.calcWasteArea() << "\n";

    //tree.writeFig(true, std::cout, 0);
    }
    return 0;
}
