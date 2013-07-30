// $Id: GraphLoaderKFile.h 94 2006-04-19 12:34:30Z bingmann $

#ifndef VGS_GraphLoaderKFile_H
#define VGS_GraphLoaderKFile_H

#include "GraphLoader.h"

#include <fstream>
#include <iostream>

namespace VGServer {

/** GraphLoaderKFile is-a GraphLoader which will load .k map files. I think
 * this doesnt really belong into the library. */

class GraphLoaderKFile : public GraphLoader
{
public:
    explicit GraphLoaderKFile(const class GraphProperties &gp)
	: GraphLoader(gp)
    {
    }

    bool load(std::istream &in, bool silent=false)
    {
	unsigned int vertexnum;
	in >> vertexnum;

	if (!silent)
	    std::cerr << "Vertices: " << vertexnum << "\n";

	for (unsigned int vnum = 0; vnum < vertexnum; vnum++)
	{
	    int vid, vx, vy;

	    // read vertex id, x pos and y pos;
	    in >> vid >> vx >> vy;

	    if (not in.good()) return false;

	    addVertex(vid);
	    setVertexAttr(vid, 0, vx);
	    setVertexAttr(vid, 1, vy);
	}
 
	unsigned int edgenum;
	in >> edgenum;

	if (!silent)
	    std::cerr << "Edges: " << edgenum << "\n";

	for (unsigned int en = 0; en < edgenum and in.good(); en++)
	{
	    int src, tgt, attr_distance, attr_direction, attr_speed;

	    in >> src >> tgt >> attr_distance >> attr_direction >> attr_speed;

	    if (not in.good()) return false;

	    addEdge(src, tgt);

	    setEdgeAttr(src, tgt, 0, attr_distance);
	    setEdgeAttr(src, tgt, 1, attr_direction);
	    setEdgeAttr(src, tgt, 2, attr_speed);
	}

	return true;
    }

    bool loadfile(const char *path, bool silent=false)
    {
	std::ifstream in(path);
	if (!in.is_open()) {
	    if (!silent)
		std::cerr << "Could not open map file " << path << std::endl;
	    return false;
	}

	return load(in, silent);
    }
};

} // namespace VGServer

#endif // VGS_GraphLoaderKFile_H
