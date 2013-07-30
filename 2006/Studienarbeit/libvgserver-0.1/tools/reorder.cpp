// $Id: reorder.cpp 45 2006-01-25 07:50:28Z bingmann $
/* Reorder a .dtmp graph map file to meet the requirements of GraphLoader.
 * Can also shift the ids to a specified offset.
 */

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <ctype.h>

// translate all vertex numbers to start at this base.
const int vertexid_offset = 0;

// number of vertex attributes following the id
const int vertexattr_copynum = 2;

// number of edge attributes following the id pair
const int edgeattr_copynum = 3;

void copy_value(std::istream &fin, std::ostream &fo)
{
    char c;

    // skip leading whitespace
    do {
	c = fin.get();
    } while( isspace(c) );

    do {
	fo.put(c);

	c = fin.get();
    }
    while( not isspace(c) );
}

std::string read_value(std::istream &fin)
{
    char c;
    std::string s;

    // skip leading whitespace
    do {
	c = fin.get();
    } while( isspace(c) );

    do {
	s += c;
	c = fin.get();
    }
    while( not isspace(c) );

    return s;
}

int reorder(std::istream &fin, std::ostream &fo)
{
    // first value in file
    unsigned int vertexnum;
    fin >> vertexnum;
    
    // reenumerate vertex ids
    unsigned int vertexid = 0;

    fo << vertexnum << std::endl;

    // maps old vertex ids to the new ones
    std::map<unsigned int, unsigned int> vertexmap;

    for(unsigned int i = 0; i < vertexnum; i++)
    {
	unsigned int vid;
	fin >> vid;
	
	if (vertexmap.find(vid) != vertexmap.end()) {
	    std::cerr << "Duplicate input vertex id: " << vid;
	    return -1;
	}

	fo << vertexid;
	vertexmap[vid] = vertexid++;
	
	// copy 2 vertex attribute values.
	for(int vi = 0; vi < vertexattr_copynum; vi++)
	{
	    fo << " ";
	    copy_value(fin, fo);
	}
	fo << std::endl;
    }

    std::cerr << "Read " << vertexnum << " vertices." << std::endl;

    // next block are the edges
    unsigned int edgenum;
    fin >> edgenum;

    // read all edges into the map thus sorting them
    typedef std::pair<unsigned int, unsigned int> EdgeId;

    typedef std::map<EdgeId, std::string> edgemap_t;
    std::map<EdgeId, std::string> edgemap;

    for(unsigned int ei = 0; ei < edgenum; ei++)
    {
	unsigned int src, tgt;
	fin >> src >> tgt;

	if (vertexmap.find(src) == vertexmap.end()) {
	    std::cerr << "Invalid source vertex id found: " << src;
	    return -1;
	}
	if (vertexmap.find(tgt) == vertexmap.end()) {
	    std::cerr << "Invalid target vertex id found: " << src;
	    return -1;
	}

	// remap vertex ids
	src = vertexmap[src];
	tgt = vertexmap[tgt];

	std::string vals;
	for(int i = 0; i < edgeattr_copynum; i++)
	{
	    vals += " ";
	    vals += read_value(fin);
	}

	if (edgemap.find(EdgeId(src,tgt)) != edgemap.end()) {
	    std::cerr << "Duplicate edge (" << src << "," << tgt << ") found." << std::endl;
	    //return -1;
	}

	edgemap[ EdgeId(src,tgt) ] = vals;
    }
    
    std::cerr << "Read and sorted " << edgemap.size() << " edges." << std::endl;

    // output edges

    fo << edgemap.size() << std::endl;

    edgemap_t::const_iterator ei;
    for(ei = edgemap.begin(); ei != edgemap.end(); ei++)
    {
	fo << ei->first.first << " " << ei->first.second;
	fo << ei->second;
	fo << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
	// use stdin
	return reorder(std::cin, std::cout);
    }
    else if (argc == 2)
    {
	std::ifstream fin(argv[1]);
	if (!fin)
	{
	    std::cerr << "Cannot open file: " << argv[1];
	    return 0;
	}
	return reorder(fin, std::cout);
    }
    else 
    {
	std::cerr << "Usage: " << argv[0] << " [infile or stdin]";
	return 0;
    }
}
