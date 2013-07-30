// $Id: randomquery.cc 207 2006-06-19 08:09:07Z bingmann $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "GraphLoaderKFile.h"
#include "ChangeTimeline.h"
#include "RTree.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sys/time.h>
#include <math.h>

//#undef R_STAR_TREE

// load the .ksnap file from std::cin, run a lot of random queries and output
// measured time.

using namespace VGServer;

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
    // load container snapshot
    GraphContainer gc (gp);

    if (!gc.loadSnapshot(std::cin)) {
	std::cerr << "Could not load snapshot\n";
	return 0;
    }
#else
/*
    // load data snapshot
    GraphData gd (gp);

    if (!gd.loadSnapshot(std::cin)) {
	std::cerr << "Could not load data snapshot\n";
	return 0;
    }
*/
#endif
    
#ifdef FIGOUT
    std::cout << "#FIG 3.2\nPortraitnCenter\nMetric\nA0\n100.00\nSingle\n-2\n10000 2\n";
#endif

#ifdef R_STAR_TREE
    for(unsigned int reinsertnum = 0; reinsertnum <= 20; reinsertnum++)
    {
	// build rtree over all levels
#endif

	std::ifstream gfile ("/tmp/bingmann/eur.kdata");
	
	GraphData gd (gp);

	if (!gd.loadSnapshot(gfile)) {
	    std::cerr << "Could not load data snapshot\n";
	    return 0;
	}
	gfile.close();

	GraphContainer gc (gp);
#ifdef R_STAR_TREE
	gc.reinsertnum = reinsertnum;
#endif

	std::cerr << "reinsert num " << gc.reinsertnum << "\n";
	gc.applyGraphData(gd);

	// compute bounding box
    
	int minX = INT_MAX,
	    minY = INT_MAX,
	    maxX = INT_MIN,
	    maxY = INT_MIN;
    
	for(vertexid_t vi = 0; vi < gc.vertices.size()-1; vi++)
	{
	    if (not gc.existVertex(vi)) continue;

	    int x = gc.getVertexAttr(vi, 0).getInteger();
	    int y = gc.getVertexAttr(vi, 1).getInteger();

	    minX = std::min(minX, x);
	    minY = std::min(minY, y);
	    maxX = std::max(maxX, x);
	    maxY = std::max(maxY, y);
	}

	std::cerr << "minX = " << minX << " - minY = " << minY << " - maxX = " << maxX << " - maxY = " << maxY << "\n";

	assert(minX <= maxX);
	assert(minY <= maxY);

#ifdef FIGOUT
	std::cout << "2 2 0 1 1 7 50 -1 -1 0.000 0 0 -1 0 0 5\n"
		  << "       "
		  << minX << " " << minY << " "
		  << maxX << " " << minY << " "
		  << maxX << " " << maxY << " "
		  << minX << " " << maxY << " "
		  << minX << " " << minY << "\n";
#endif

	ChangeTimeline ct(gc);

	ByteBuffer bb;
	bb.alloc(10*1024*1024);

	double totaltime = 0.0;
	long long totaledge = 0;
	srand48(109859038);

	// determine random regions within bounds
	for(unsigned int randreg = 0; randreg < 10000; randreg++)
	{
#if 0
	    int x1 = (lrand48() % (maxX - minX)) + minX;
	    int x2 = (lrand48() % (maxX - minX)) + minX;
	    if (x1 > x2) {
		std::swap(x1,x2);
	    }

	    // totally random box forms
	    int y1 = (lrand48() % (maxY - minY)) + minY;
	    int y2 = (lrand48() % (maxY - minY)) + minY;
	    if (y1 > y2) {
		std::swap(y1,y2);
	    }
#elsif 0
	    int x1 = (lrand48() % (maxX - minX)) + minX;
	    int x2 = (lrand48() % (maxX - minX)) + minX;
	    if (x1 > x2) {
		std::swap(x1,x2);
	    }

	    // random boxes with forms like a client view area
	    int y1 = (lrand48() % (maxY - minY)) + minY;
	    double ratio = 0.5 + (lrand48() % 65536) / (65536.0/0.7);

	    int y2 = (int)(y1 + (x2 - x1) * ratio);
	    if (y1 > y2) {
		std::swap(y1,y2);
	    }
#else
	    // random boxes with fixed ratio and different areas
	    // area range: 10^7 - 10^11
	    double area = drand48() * (double)std::min(maxX - minX, maxY - minY);

	    int width = int(area);
	    int height = int(area);
	    int x1, y1, x2, y2;

	    while(1)
	    {
		x1 = (lrand48() % (maxX - minX)) + minX;
		y1 = (lrand48() % (maxY - minY)) + minY;

		if (x1 + width <= maxX) {
		    x2 = x1 + width;
		}
		else if (x1 - width >= minX) {
		    x2 = x1 - width;
		    std::swap(x1,x2);
		}
		else {
		    continue;
		}

		if (y1 + width <= maxY) {
		    y2 = y1 + height;
		}
		else if (y1 - height >= minY) {
		    y2 = y1 - height;
		    std::swap(y1,y2);
		}
		else {
		    continue;
		}

		break;
	    }
#endif

	    //std::cerr << "range (" << x1 << "," << y1 << ") - (" << x2 << "," << y2 << ")\n";
	
#ifdef FIGOUT
	    std::cout << "2 2 0 1 0 7 50 -1 -1 0.000 0 0 -1 0 0 5\n"
		      << "       "
		      << x1 << " " << y1 << " "
		      << x2 << " " << y1 << " "
		      << x2 << " " << y2 << " "
		      << x1 << " " << y2 << " "
		      << x1 << " " << y1 << "\n";
	    continue;
#endif

	    double ts1 = timestamp();
	
	    ByteBuffer bb;

	    const int loops = 1;

	    for(unsigned int i = 0; i < loops; i++)
	    {
		gc.getArea(x1, y1, x2, y2, NULL,
			   "",
			   "","(src.x - 200000) * 0.03, src.y, tgt.x, tgt.y, speed", ct, bb);
	    }

	    double ts2 = timestamp();
	    double qt = (ts2-ts1) / (double)loops;

	    // std::cerr << "Query time: " << qt << " - edges: " << gc.getLastQueryEdgenum() << "\n";

	    //std::cout << ((long long)(x2 - x1) * (long long)(y2 - y1)) << " " << gc.getLastQueryEdgenum() << " " << qt << "\n";

	    totaltime += qt;
	    totaledge += gc.getLastQueryEdgenum();
	}

	std::cerr << "Total time: " << totaltime << ", edges = " << totaledge << ", edge/time = " << std::fixed << std::setprecision(2) << (totaledge / totaltime) << "\n";

#ifdef R_STAR_TREE
    }
#endif    
    return 0;
}
