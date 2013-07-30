// $Id: test1.cc 242 2006-07-05 14:54:52Z schultes $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "Changelist.h"
#include "ChangeTimeline.h"
#include "ByteOutBuffer.h"
#include "AttributeBlob.h"
#include "GraphLoaderKFile.h"
#include "GraphServer.h"

#include "RTree.h"

#include "AttributeParser.h"

#include "ByteInBuffer.h"
#include "GraphParser.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#if defined(__GNUC__)
#include <sys/time.h>

double timestamp() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return double(tp.tv_sec) + tp.tv_usec / 1000000.;
}
#else
double timestamp() {
    return 0.0;
}
#endif

class TestParserCallback : public VGServer::GraphParserCallbackAdapter
{
public:

    void eventStartEdgeBody(unsigned int objnum)
    {
	std::cout << "Parser: " << objnum << " edges\n";
    }
    void eventEdge(const std::vector<VGServer::AnyType> &vals)
    {
	//std::cout << "val0: " << vals[0].getDouble() << "\n";
    }

};

int main(int argc, char *argv[])
{
    using namespace VGServer;

    srand(time(NULL));

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
    std::string arg(argv[1]);
    AttributeSelectorList froot;

    try {
	froot.parseString(arg, gp, VE_EDGE);
    }
    catch(VGServer::ConversionException &e)
    {
	std::cout << "ConversionEx: " << e.what() << "\n";
    }
    catch(VGServer::ParseException &e)
    {
	std::cout << "ParseEx: " << e.what() << "\n";
    }

    std::cout << "Parsed: " << froot.toString() << "\n";

    return 0;
#endif

#if 0
    GraphLoaderKFile loader(gp);

    if (!loader.load(std::cin)) return 0;

    GraphContainer gc(gp);

    gc.applyGraphData( loader.finish() );
    
    assert( gc.checkReferences() );

    gc.saveSnapshot(std::cout);
    return 0;
#endif
#if 1
    GraphContainer gc(gp);

    std::ifstream fin("karten/deu.ksnap");

    if (!gc.loadSnapshot(fin)) {
	std::cerr << "Loading of snapshot failed.\n";
	return 0;
    }

    assert( gc.checkReferences() );
#endif

    ChangeTimeline ct (gc);

#if 1
   double ats1 = timestamp();

    int chg = 0;

    std::vector<class EdgeRef> edgelist;

    for(int li = 0; li < 100; li++)
    {
	ct.advanceTimeFrame();

	int vertexnum = gc.getVertexCount();
	int randnum = 10000; //int( vertexnum * 1.0 );

/*
	for(int i = 0; i < vertexnum; i++)
	{
	    unsigned int vid = i; //rand() % 10000;

	    cl.addVertex(vid);

	    cl.setVertexAttr(vid, 0, rand() % 10000);
	    cl.setVertexAttr(vid, 1, rand() % 10000);
	}
*/

	for(int i = 0; i < randnum; i++)
	{
	    int vid1 = rand() % vertexnum;
	    vid1 = i;

	    edgelist = gc.getEdgeList(vid1, ct.getChangelistEnd());
	    if (edgelist.size() == 0) {
		randnum++;
		continue;
	    }

	    if (ct.setEdgeAttr(edgelist[0].getSource(),
			       edgelist[0].getTarget(),
			       2,
			       110))
	    {
		chg++;
	    }
	}
	std::cerr << "Changes: " << chg << "\n";
     }

    double ats2 = timestamp();
    
    std::cout << "Change time: " << (ats2-ats1) << "\n";

    //gd.writeFig(std::cout, 0, 1);
#endif

#if 1
    //GraphContainer gcx (gp);
    //gcx.swap(gc);

    //ChangeTimeline ct(gcx);

    double ts1 = timestamp();

    ByteBuffer bb;

    for(unsigned int i = 0; i < 1; i++)
    {
	gc.getArea(0,0,10000000,10000000, NULL,
		   "","","(src.x - 200000) * 0.03, src.y, tgt.x, tgt.y, speed", ct, bb);
    }

    double ts2 = timestamp();

    std::cerr << "Query time: " << (ts2-ts1) << " sec. Result size: " << bb.size() << std::endl;

    TestParserCallback cb;

    GraphParser<TestParserCallback>::parse(cb, bb);
#endif

#if 0
    Changelist cl(gc);

    double ts1 = timestamp();
    
    ByteOutBuffer bob = gc.getNearestNeighbor(0,0,1000000,10000000, 843761, 489432, 
					      "", "*", "*", cl);

    double ts2 = timestamp();

    std::cout << "Query time: " << (ts2-ts1)/20.0 << " sec. Result size: " << bob.size() << std::endl;
#endif

#if 0
    RTree tree;

    for(vertexid_t vi = 0; vi < gd.vertices.size()-1; vi++)
    {
	if (not gd.existVertex(vi)) continue;

	int x1 = gd.getVertexAttr(vi, 0).getInteger();
	int y1 = gd.getVertexAttr(vi, 1).getInteger();

	unsigned int endedgeidx = gd.vertices[vi+1].edgeidx;

	for(unsigned int ei = gd.vertices[vi].edgeidx; ei < endedgeidx; ei++)
	{
	    vertexid_t tvi = gd.edges[ei].target;
	    if (tvi == VERTEX_INVALID) break;
	    if (not gd.existVertex(tvi)) continue;

	    int x2 = gd.getVertexAttr(tvi, 0).getInteger();
	    int y2 = gd.getVertexAttr(tvi, 1).getInteger();

	    // unsigned int speed = gd.edges[ei].getAttr(2, gd).getInteger();
	    // if (speed != 11) continue;

	    tree.insertRect(RTree::Rect(std::min(x1,x2), std::min(-y1,-y2),
					std::max(x1,x2), std::max(-y1,-y2)),
			    RTreeData(0,0));
	    //assert(tree.testTree());
	}
    }

    assert(tree.testTree());

#if 0
    for(vertexid_t vi = 0; vi < gd.vertices.size()-1; vi++)
    {
	if (not gd.existVertex(vi)) continue;

	unsigned int x1 = gd.getVertexAttr(vi, 0).getInteger();
	unsigned int y1 = gd.getVertexAttr(vi, 1).getInteger();

	unsigned int endedgeidx = gd.vertices[vi+1].edgeidx;

	for(unsigned int ei = gd.vertices[vi].edgeidx; ei < endedgeidx; ei++)
	{
	    vertexid_t tvi = gd.edges[ei].target;
	    if (tvi == VERTEX_INVALID) break;
	    if (not gd.existVertex(tvi)) continue;

	    unsigned int x2 = gd.getVertexAttr(tvi, 0).getInteger();
	    unsigned int y2 = gd.getVertexAttr(tvi, 1).getInteger();

	    // unsigned int speed = gd.edges[ei].getAttr(2, gd).getInteger();
	    // if (speed != 11) continue;

	    bool b = tree.deleteRect(RTree::Rect(std::min(x1,x2), std::min(-y1,-y2),
						 std::max(x1,x2), std::max(-y1,-y2)),
				     0);
	    
	    assert(b);
	    //assert(tree.testTree());
	}
    }

    assert(tree.testTree());
    assert(tree.empty());
#endif

    std::ofstream ffig("o.fig");

    //gd.writeFig(true, ffig, 0, 1);

    tree.writeFig(true, ffig, 1);

#endif
}
