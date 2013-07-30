// $Id: AttributeParserTest.cc 238 2006-06-29 17:28:25Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "AttributeParser.h"
#include "GraphProperties.h"

#include <stdlib.h>
#include <iostream>

using namespace VGServer;

class AttributeParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AttributeParserTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	GraphProperties gp;
	gp.vertexattr.push_back(AttributeProperties("x", int(0) ));
	gp.vertexattr.push_back(AttributeProperties("y", int(0) ));

	gp.edgeattr.push_back(AttributeProperties("speed", int(0) ));

	gp.calcAttributeLookups();

	// a few selector strings

	AttributeSelectorList asl;

	CPPUNIT_ASSERT(asl.parseString("x, y, 500 + x, 100 + 200 - y, 100 == 300, y == 10", gp, VE_VERTEX));
	
	CPPUNIT_ASSERT(asl.toString() == "x, y, (500 + x), (300 - y), false, (y == 10)");

	CPPUNIT_ASSERT_THROW(asl.parseString("z", gp, VE_VERTEX), AttributeParseException);

	CPPUNIT_ASSERT_THROW(asl.parseString("(src.x - 300) * 0.040, speed", gp, VE_VERTEX), AttributeParseException);

	CPPUNIT_ASSERT(asl.parseString("(src.x - 300) * 0.040, speed", gp, VE_EDGE));

	CPPUNIT_ASSERT(asl.toString() == "((src.x - 300) * 0.04), speed");

	CPPUNIT_ASSERT(asl.parseString("(src.x - 815000) * (0.01582) CAST SHORT,(src.y - 4870000) * (0.01076) CAST SHORT,(tgt.x - 815000) * (0.01582) CAST SHORT,(tgt.y - 4870000) * (0.01076) CAST SHORT,speed", gp, VE_EDGE));

	CPPUNIT_ASSERT(asl.toString() == "(((src.x - 815000) * 0.02) cast short), (((src.y - 4870000) * 0.01) cast short), (((tgt.x - 815000) * 0.02) cast short), (((tgt.y - 4870000) * 0.01) cast short), speed");

	// some filters

	FilterRoot fr;

	CPPUNIT_ASSERT(fr.parseString("", gp));

	CPPUNIT_ASSERT(fr.parseString("vertex: x < 200000 && y < 100000", gp));

	CPPUNIT_ASSERT(fr.toString() == "vertex: ((x < 200000) && (y < 100000))");
	
	CPPUNIT_ASSERT_THROW(fr.parseString("x < 200", gp), AttributeParseException);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AttributeParserTest );
