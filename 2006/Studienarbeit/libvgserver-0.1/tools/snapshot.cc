// $Id: snapshot.cc 148 2006-05-24 14:52:39Z bingmann $

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "GraphLoaderKFile.h"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
    using namespace VGServer;

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

    // load the .k file from std::cin and write the snapshot to std::cout

    GraphLoaderKFile loader(gp);

    if (!loader.load(std::cin)) return 0;

    if (argc == 2 && strcmp(argv[1], "data") == 0)
    {
	// write out GraphData snapshot
	loader.finish().saveSnapshot(std::cout);
	return 0;
    }

    GraphContainer gc(gp);

    gc.applyGraphData( loader.finish() );
    
    assert( gc.checkReferences() );

    gc.saveSnapshot(std::cout);
    return 0;
}
