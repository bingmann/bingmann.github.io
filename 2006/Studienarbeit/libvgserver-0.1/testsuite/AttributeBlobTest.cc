// $Id: AttributeBlobTest.cc 101 2006-04-23 08:19:17Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "AttributeBlob.h"
#include "GraphProperties.h"

#include "AttributeBlob_impl.h"

#include <stdlib.h>
#include <iostream>

using namespace VGServer;

class AttributeBlobTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AttributeBlobTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST(test2);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	AttributeVertexTinyBlob ab;

	ab.put<unsigned int>(0, 0x50);
	ab.put<unsigned char>(4, 0x30);

	AnyType i1 (0x401);
	ab.putAnyType(5, i1);


	CPPUNIT_ASSERT( ab.get<unsigned char>(0) == 0x50 );
	CPPUNIT_ASSERT( ab.get<unsigned int>(4) == 0x40130 );
    }

    void test2()
    {
	GraphProperties gp;

	gp.vertexattr.push_back(AttributeProperties("x", AnyType(int(0x10)) ));
	gp.vertexattr.push_back(AttributeProperties("y", AnyType(short(0x20)) ));
	gp.vertexattr.push_back(AttributeProperties("z", AnyType(int(50)) ));
	gp.vertexattr.push_back(AttributeProperties("bv", AnyType(bool(false)) ));
	gp.vertexattr.push_back(AttributeProperties("level", AnyType(double(5.0)) ));

	gp.vertexattr[0].varying = true;
	gp.vertexattr[1].varying = true;

	gp.calcAttributeLookups();
	
	AttributeVertexTinyBlob ab = gp.vertexattr.createBlankAttributeBlob<AttributeVertexTinyBlob>();

	// test the result of the blank blob initialization
	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 7 );

	CPPUNIT_ASSERT( ab.getRange(0, 7) == std::string("\x05\x10\x00\x00\x00\x20\x00", 7) );

	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 0, gp.vertexattr) == AnyType(int(16)) );
	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 1, gp.vertexattr) == AnyType(short(32)) );
	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 2, gp.vertexattr) == AnyType(int(50)) );
	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 3, gp.vertexattr) == AnyType(bool(false)) );
	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 4, gp.vertexattr) == AnyType(double(5.0)) );

	// put a double value at the end
	ab.putAttrChainValue(gp.vertexattr, 4, AnyType(short(20)));

	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 15 );

	CPPUNIT_ASSERT( ab.getRange(0, 15) == std::string("\x01\x10\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x34\x40", 15) );

	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 4, gp.vertexattr) == AnyType(double(20.0)) );

	// insert a value for the z attribute
	ab.putAttrChainValue(gp.vertexattr, 2, AnyType(double(30.0)));

	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 19 );
	
	CPPUNIT_ASSERT( ab.getRange(0, 19) == std::string("\x00\x10\x00\x00\x00\x20\x00\x1e\0\0\0\0\0\0\0\0\0\x34\x40", 19) );

	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 2, gp.vertexattr) == AnyType(int(30)) );
	
	// change a varying attribute from the default
	ab.putAttrChainValue(gp.vertexattr, 1, AnyType(0x30));

	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 19 );
	CPPUNIT_ASSERT( ab.getRange(0, 19) == std::string("\x00\x10\x00\x00\x00\x30\x00\x1e\0\0\0\0\0\0\0\0\0\x34\x40", 19) );

	// set a boolean value
	ab.putAttrChainValue(gp.vertexattr, 3, AnyType(1));

	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 19 );
	CPPUNIT_ASSERT( ab.getRange(0, 19) == std::string("\x02\x10\x00\x00\x00\x30\x00\x1e\0\0\0\0\0\0\0\0\0\x34\x40", 19) );

	// set an attribute to the default
	ab.putAttrChainValue(gp.vertexattr, 2, AnyType(50.0));

	CPPUNIT_ASSERT( ab.getAttrChainLength(0, gp.vertexattr) == 15 );
	
	CPPUNIT_ASSERT( ab.getRange(0, 15) == std::string("\x03\x10\x00\x00\x00\x30\x00\0\0\0\0\0\0\x34\x40", 15) );

	CPPUNIT_ASSERT( ab.getAttrChainValue(0, 2, gp.vertexattr) == AnyType(int(50)) );

	//std::cout.write(ab.data(), ab.size());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AttributeBlobTest );
