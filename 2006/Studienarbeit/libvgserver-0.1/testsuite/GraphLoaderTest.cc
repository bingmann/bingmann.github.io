// $Id: GraphLoaderTest.cc 47 2006-01-26 19:08:04Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "GraphData.h"
#include "GraphProperties.h"
#include "GraphLoader.h"

#include <stdlib.h>
#include <sstream>

using namespace VGServer;

class GraphLoaderTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( GraphLoaderTest );
    CPPUNIT_TEST(test_trivial_load);
    //CPPUNIT_TEST(test1);
    //CPPUNIT_TEST(test2);
    CPPUNIT_TEST_SUITE_END();

protected:

    // use the graph loader to "load" an empty graph
    void test_trivial_load()
    {
	GraphProperties gp;
	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
	gp.calcAttributeLookups();
	
	GraphLoader gl (gp);

	// load nothing

	GraphData &gd = gl.finish();

	CPPUNIT_ASSERT( gd.checkReferences() );

	CPPUNIT_ASSERT( gd.getVertexCount() == 0 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 0 );
    }

    void test1()
    {
    }

    void test2()
    {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( GraphLoaderTest );
