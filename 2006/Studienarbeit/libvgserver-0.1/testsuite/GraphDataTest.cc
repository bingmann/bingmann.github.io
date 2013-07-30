// $Id: GraphDataTest.cc 192 2006-06-15 08:00:53Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "GraphData.h"
#include "GraphProperties.h"
#include "Changelist.h"
#include "GraphLoaderKFile.h"

#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace VGServer;

class GraphDataTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( GraphDataTest );
    CPPUNIT_TEST(test_trivial_changelist);
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST(test2);
    CPPUNIT_TEST(test3);
    CPPUNIT_TEST_SUITE_END();

protected:

    // apply the trivial change list to an empty GraphData object
    void test_trivial_changelist()
    {
	GraphProperties gp;
	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
	gp.calcAttributeLookups();
	
	GraphData gd (gp);
	CPPUNIT_ASSERT( gd.checkReferences() );

	{
	    Changelist cl (gd);

	    gd.applyChangelist(cl);
	}

	CPPUNIT_ASSERT( gd.checkReferences() );

	CPPUNIT_ASSERT( gd.getVertexCount() == 0 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 0 );

	{
	    Changelist cl (gd);

	    cl.addVertex(0);
	    cl.setVertexAttr(0, 0, AnyType(50));
	    cl.setVertexAttr(0, 1, AnyType(50));

	    gd.applyChangelist(cl);
	}

	CPPUNIT_ASSERT( gd.checkReferences() );

	CPPUNIT_ASSERT( gd.getVertexCount() == 1 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 0 );

	{
	    Changelist cl (gd);

	    cl.addVertex(1);
	    cl.setVertexAttr(1, 0, AnyType(70));
	    cl.setVertexAttr(1, 1, AnyType(70));

	    cl.addEdge(0,1);

	    gd.applyChangelist(cl);
	}

	CPPUNIT_ASSERT( gd.checkReferences() );

	CPPUNIT_ASSERT( gd.getVertexCount() == 2 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 1 );
    }

    void test1()
    {
	GraphProperties gp;
	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
	gp.calcAttributeLookups();

	int maxvertexid = 100;

	GraphData gd(gp);

	srand(34236);

	for(int i = 0; i < 10; i++)
	{
	    Changelist cl(gd);

	    for(int i = 0; i < 10; i++)
	    {
		unsigned int vid = rand() % maxvertexid;
		cl.addVertex(vid);

		cl.setVertexAttr(vid, 0, AnyType(rand() % 10000));
		cl.setVertexAttr(vid, 1, AnyType(rand() % 10000));
	    }

	    for(int i = 0; i < 100; i++)
	    {
		cl.addEdge(rand() % maxvertexid,
			   rand() % maxvertexid);
	    }

	    gd.applyChangelist(cl);
	}

	std::ostringstream so;
	gd.writeFig(true, so, 0, 1);
	
	//std::cerr << "std: "<< so.tellp();
	CPPUNIT_ASSERT( so.tellp() == std::streampos(15954) );
    }

    void test2()
    {
	GraphProperties gp;
	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
	gp.calcAttributeLookups();

	int maxvertexid = 10000;

	GraphData gd(gp);

	srand(342364);

	for(int i = 0; i < 100; i++)
	{
	    Changelist cl(gd);

	    for(int i = 0; i < 100; i++)
	    {
		unsigned int vid = rand() % maxvertexid;
		cl.addVertex(vid);

		cl.setVertexAttr(vid, 0, AnyType(rand() % 10000));
		cl.setVertexAttr(vid, 1, AnyType(rand() % 10000));
	    }

	    for(int i = 0; i < 100; i++)
	    {
		cl.addEdge(rand() % maxvertexid,
			   rand() % maxvertexid);
	    }

	    gd.applyChangelist(cl);
	}

	std::ostringstream so;
	gd.writeFig(true, so, 0, 1);

	//std::cerr << "std: "<< so.tellp();
	CPPUNIT_ASSERT( so.tellp() == std::streampos(523971) );
    }

    void test3()
    {
	GraphProperties gp;

	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(int(0)) ));
	gp.vertexattr.push_back(AttributeProperties("level", AnyType(int(0)) ));

	gp.vertexattr[0].varying = true;
	gp.vertexattr[1].varying = true;

	gp.edgeattr.push_back(AttributeProperties("distance", AnyType(int(0)) ));
	gp.edgeattr.push_back(AttributeProperties("direction", AnyType(char(0)) ));
	gp.edgeattr.push_back(AttributeProperties("speed", AnyType(char(0)) ));

	gp.calcAttributeLookups();

	GraphLoaderKFile loader(gp);

	std::fstream fin("lux.k");
	CPPUNIT_ASSERT( fin );

	CPPUNIT_ASSERT( loader.load(fin) );

	GraphData& gd = loader.finish();

	CPPUNIT_ASSERT( gd.getVertexCount() == 30746 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 38143 );

	Changelist cl (gd);

	for(unsigned int vi = 0; vi < gd.getVertexCount(); vi++)
	{
	    CPPUNIT_ASSERT( gd.existVertex(vi) );

	    CPPUNIT_ASSERT( cl.setVertexAttr(vi, 2, 10) );
	}
	
	gd.applyChangelist(cl);

	CPPUNIT_ASSERT( gd.getVertexCount() == 30746 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 38143 );

	for(unsigned int vi = 0; vi < gd.getVertexCount(); vi++)
	{
	    CPPUNIT_ASSERT( gd.existVertex(vi) );
	    CPPUNIT_ASSERT( gd.getVertexAttr(vi, 2).getInteger() == 10 );
	}

	cl.clear();

	std::vector<const GraphData::Edge*> edgevec;

	for(unsigned int v1 = 0; v1 < gd.getVertexCount(); v1++)
	{
	    edgevec = gd.getEdgeList(v1);

	    for(unsigned int ei = 0; ei < edgevec.size(); ei++)
	    {
		CPPUNIT_ASSERT( gd.existEdge(v1, edgevec[ei]->target) );

		CPPUNIT_ASSERT( cl.setEdgeAttr(v1, edgevec[ei]->target, 1, 5) ); 
	    }
	}

	CPPUNIT_ASSERT( cl.setEdgeAttr(100000,2000000, 1, 5) == false );

	gd.applyChangelist(cl);

	CPPUNIT_ASSERT( gd.getVertexCount() == 30746 );
	CPPUNIT_ASSERT( gd.getEdgeCount() == 38143 );

	for(unsigned int v1 = 0; v1 < gd.getVertexCount(); v1++)
	{
	    edgevec = gd.getEdgeList(v1);

	    for(unsigned int ei = 0; ei < edgevec.size(); ei++)
	    {
		CPPUNIT_ASSERT( gd.existEdge(v1, edgevec[ei]->target) );

		CPPUNIT_ASSERT( gd.getEdgeAttr(v1, edgevec[ei]->target, 1).getInteger() == 5 ); 
	    }
	}
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( GraphDataTest );
