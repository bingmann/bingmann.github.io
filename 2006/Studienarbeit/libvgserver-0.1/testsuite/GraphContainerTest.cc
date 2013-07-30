// $Id: GraphContainerTest.cc 47 2006-01-26 19:08:04Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "GraphContainer.h"
#include "GraphProperties.h"
#include "ChangeTimeline.h"
#include "ByteOutBuffer.h"

#include <stdlib.h>

#include <boost/crc.hpp>

class GraphContainerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( GraphContainerTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	VGServer::GraphProperties GP;
	GP.directed = false;

/*

	VGServer::GraphContainer GC(GP);
	VGServer::ChangeTimeline CL;
    
	CL.addVertex(1, 500, 200);
	CL.addVertex(2, 200, 10);
	CL.addVertex(3, 100, 900);
	CL.addVertex(4, 900, 100);
	CL.addVertex(5, 700, 800);

	CL.addEdge(2,3);
	CL.addEdge(4,5);
	CL.addEdge(1,4);
	CL.addEdge(4,2);
    
	GC.applyChangelist(CL.getChangelist());

	CL.clear();

	std::string bin = GC.getArea(0,0, 1000,1000, NULL, NULL, NULL, CL.getChangelist());

	// test crc instead of putting binary output here.
	boost::crc_32_type crc32;
	crc32.process_bytes(bin.data(), bin.size());
	CPPUNIT_ASSERT( crc32.checksum() == 3715907100LU );
*/
    }
};

//CPPUNIT_TEST_SUITE_REGISTRATION( GraphContainerTest );
