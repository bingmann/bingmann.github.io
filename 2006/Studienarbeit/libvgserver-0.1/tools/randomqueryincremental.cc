// $Id: randomqueryincremental.cc 207 2006-06-19 08:09:07Z bingmann $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "GraphLoaderKFile.h"
#include "ChangeTimeline.h"
#include "RTree.h"
#include "Changelist.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sys/time.h>

// load the .ksnap file from std::cin, run a lot of random queries and output
// measured time.

using namespace VGServer;

inline double timestamp()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return double(tp.tv_sec) + tp.tv_usec / 1000000.;
}

const struct QueryLimits infinitelimits(1000000000, 1000000000, 1000000000, 1000000000);

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

    // load only vertices
    std::istream &in = std::cin;
    GraphLoader loader (gp);
    {
	unsigned int vertexnum;
	in >> vertexnum;

	std::cerr << "Vertices: " << vertexnum << "\n";

	for (unsigned int vnum = 0; vnum < vertexnum; vnum++)
	{
	    int vid, vx, vy;

	    // read vertex id, x pos and y pos;
	    in >> vid >> vx >> vy;

	    if (not in.good()) return false;

	    loader.addVertex(vid);
	    loader.setVertexAttr(vid, 0, vx);
	    loader.setVertexAttr(vid, 1, vy);
	}
    }

    // create container
    GraphContainer gc (gp);

    gc.applyGraphData(loader.finish());

    // load edges incrementally
    unsigned int totaledgenum;
    in >> totaledgenum;

    std::cerr << "Edges: " << totaledgenum << "\n";

    while(gc.getEdgeCount() < totaledgenum)
    {
	std::cerr << "Loading next edges batch\n";
	{
	    Changelist cl (gc);

	    for (unsigned int en = 0; en < 50000 and in.good(); en++)
	    {
		int src, tgt, attr_distance, attr_direction, attr_speed;

		in >> src >> tgt >> attr_distance >> attr_direction >> attr_speed;

		if (not in.good()) {
		    std::cerr << "End of edges\n";
		    break;
		}

		if (!cl.addEdge(src, tgt)) {
		    std::cerr << "addEdge failed: " << src << " " << tgt << "\n";
		}

		if (!cl.setEdgeAttr(src, tgt, 0, attr_distance) or
		    !cl.setEdgeAttr(src, tgt, 1, attr_direction) or
		    !cl.setEdgeAttr(src, tgt, 2, attr_speed))
		{
		    std::cerr << "setEdgeAttr Failed\n";
		}
	    }
	    
	    std::cerr << "Merging\n";
	    gc.applyChangelist(cl);

	    std::cerr << "Now " << gc.getEdgeCount() << " edges in graph\n";
	}

	std::cerr << "Random Queries\n";
	{
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

	    ChangeTimeline ct(gc);

	    ByteBuffer bb;
	    bb.alloc(10*1024*1024);

	    const int batches = 5;

	    long totalruns[batches];
	    double totaltime[batches];
	    long long totaledge[batches];
	    
	    memset(totalruns, 0, sizeof(totalruns));
	    memset(totaltime, 0, sizeof(totaltime));
	    memset(totaledge, 0, sizeof(totaledge));

	    srand48(109859038);

	    // determine random regions within bounds
	    for(unsigned int randreg = 0; randreg < 1040; randreg++)
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

		int countreg = 0;
		const struct QueryLimits *limits = NULL;

		if (randreg % 100 == 55)
		{
		    countreg = 1;

		    // nah ausschnitt karlsruhe
		    x1 =  837420;
		    y1 = 4899440;

		    x2 =  843500;
		    y2 = 4903350;

		    limits = &infinitelimits;
		}
		else if (randreg % 100 == 79)
		{
		    countreg = 2;

		    // grossraum karlsruhe
		    x1 =  831000;
		    y1 = 4893940;

		    x2 =  851360;
		    y2 = 4907040;

		    limits = &infinitelimits;
		}
		else if (randreg % 100 == 46)
		{
		    countreg = 3;

		    // badenwuerttemberg
		    x1 =  753720;
		    y1 = 4745700;

		    x2 = 1047310;
		    y2 = 4934620;

		    limits = &infinitelimits;
		}
		else if (randreg % 100 == 90)
		{
		    countreg = 4;

		    // deutschland
		    x1 =  586663;
		    y1 = 4727768;

		    x2 = 1503527;
		    y2 = 5504569;

		    limits = &infinitelimits;
		}

		//std::cerr << "range (" << x1 << "," << y1 << ") - (" << x2 << "," << y2 << ")\n";
		/*
		  std::cout << "2 2 0 1 0 7 50 -1 -1 0.000 0 0 -1 0 0 5\n"
		  << "       "
		  << x1 << " " << y1 << " "
		  << x2 << " " << y1 << " "
		  << x2 << " " << y2 << " "
		  << x1 << " " << y2 << " "
		  << x1 << " " << y1 << "\n";
		*/

		double ts1 = timestamp();
	
		ByteBuffer bb;

		const int loops = 1;

		for(unsigned int i = 0; i < loops; i++)
		{
		    gc.getArea(x1, y1, x2, y2, limits,
			       "",
			       "","(src.x - 200000) * 0.03, src.y, tgt.x, tgt.y, speed", ct, bb);
		}

		double ts2 = timestamp();
		double qt = (ts2-ts1) / (double)loops;

		//std::cerr << "Query time: " << qt << " - edges: " << areaedgenum << "\n";

		//std::cout << ((long long)(x2 - x1) * (long long)(y2 - y1)) << " " << areaedgenum << " " << qt << "\n";

		if (countreg >= 0) {
		    totalruns[countreg]++;
		    totaltime[countreg] += qt;
		    totaledge[countreg] += gc.getLastQueryEdgenum();
		}
	    }

	    for (unsigned int r = 0; r < batches; r++)
	    {
		std::cerr << "Region " << r
			  << " runs = " << totalruns[r]
			  << " total time = " << std::fixed << std::setprecision(4) << totaltime[r]
			  << " edges: " << totaledge[r]
			  << " edge/time = " << std::fixed << std::setprecision(2) << (totaledge[r] / totaltime[r])
			  << "\n";
	    }
	}
    }

    return 0;
}
