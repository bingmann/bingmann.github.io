// $Id: GraphPropertiesTest.cc 127 2006-05-13 15:30:07Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "GraphProperties.h"
#include "ChangeTimeline.h"
#include "ByteOutBuffer.h"

#include <stdlib.h>
#include <math.h>

#include <iostream>

class GraphPropertiesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( GraphPropertiesTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	VGServer::GraphProperties gp;

	std::string in("directed\n"
		       "/* comments */\n"
		       "vertexattr (\n"
		       "  kategorie	integer		default 4,\n"
		       "  prioritaet	char,\n"
		       "  size		integer		default 10,\n"
		       "  /* this is the name */\n"
		       "  name		string		default \"none\"\n"
		       ")\n"
		       "\n"
		       "edgeattr (\n"
		       "  x		integer		varying,\n"
		       "  y		integer		varying,\n"
		       "  kategorie	char		default 3,\n"
		       "  prioritaet	char		default 0,\n"
		       "  laenge	float		default 4.3\n"
		       ")\n"
	    );

	CPPUNIT_ASSERT( gp.parseConfigString(in) );
	
	// check parsed values
	CPPUNIT_ASSERT( gp.directed == true );	

	CPPUNIT_ASSERT( gp.vertexattr.size() == 4 );

	CPPUNIT_ASSERT( gp.vertexattr[0].name == "kategorie" );
	CPPUNIT_ASSERT( gp.vertexattr[1].name == "prioritaet" );
	CPPUNIT_ASSERT( gp.vertexattr[2].name == "size" );
	CPPUNIT_ASSERT( gp.vertexattr[3].name == "name" );

	CPPUNIT_ASSERT( gp.vertexattr[0].getType() == VGServer::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( gp.vertexattr[1].getType() == VGServer::ATTRTYPE_CHAR );
	CPPUNIT_ASSERT( gp.vertexattr[2].getType() == VGServer::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( gp.vertexattr[3].getType() == VGServer::ATTRTYPE_STRING );

	CPPUNIT_ASSERT( gp.vertexattr[0].getInteger() == 4 );
	CPPUNIT_ASSERT( gp.vertexattr[2].getInteger() == 10 );
	CPPUNIT_ASSERT( gp.vertexattr[3].getString() == "none" );

	CPPUNIT_ASSERT( gp.edgeattr.size() == 5 );

	CPPUNIT_ASSERT( gp.edgeattr[0].name == "x" );
	CPPUNIT_ASSERT( gp.edgeattr[1].name == "y" );
	CPPUNIT_ASSERT( gp.edgeattr[2].name == "kategorie" );
	CPPUNIT_ASSERT( gp.edgeattr[3].name == "prioritaet" );
	CPPUNIT_ASSERT( gp.edgeattr[4].name == "laenge" );

	CPPUNIT_ASSERT( gp.edgeattr[0].getType() == VGServer::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( gp.edgeattr[1].getType() == VGServer::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( gp.edgeattr[2].getType() == VGServer::ATTRTYPE_CHAR );
	CPPUNIT_ASSERT( gp.edgeattr[3].getType() == VGServer::ATTRTYPE_CHAR );
	CPPUNIT_ASSERT( gp.edgeattr[4].getType() == VGServer::ATTRTYPE_FLOAT );

	CPPUNIT_ASSERT( fabs(gp.edgeattr[4].getDouble() - 4.3) < 0.1 );

	gp.calcAttributeLookups();

	CPPUNIT_ASSERT( gp.vertexattr[0].lookup < 0 );
	CPPUNIT_ASSERT( gp.edgeattr[0].lookup == 1 );
	CPPUNIT_ASSERT( gp.edgeattr[1].lookup == 5 );
	CPPUNIT_ASSERT( gp.edgeattr[2].lookup < 0 );

	VGServer::ByteBuffer bb;
	VGServer::ByteOutBuffer bob (bb);
	gp.vertexattr.appendBinaryFormat(bob);

	CPPUNIT_ASSERT( bb.toString() == std::string("\x04\x00\x01\x12\x09kategorie\x04\x00\x00\x00\x10\x0Aprioritaet\x00\x12\x04size\x0A\x00\x00\x00\x40\x04name\x04none",52) );

	bb.clear();
	gp.edgeattr.appendBinaryFormat(bob);

	std::string so = std::string("\x005\x000\x001\x012\x001x\x000\x000\x000\x000\x012\x001y\x000\x000\x000\x000\x010\x009kategorie\x003\x010\x00Aprioritaet\x000""0\x006laenge\x09A\x099\x089\x040",54);

	CPPUNIT_ASSERT( bb.toString() == so  );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( GraphPropertiesTest );
