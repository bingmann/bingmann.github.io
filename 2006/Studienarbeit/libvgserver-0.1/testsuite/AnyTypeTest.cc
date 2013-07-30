// $Id: AnyTypeTest.cc 192 2006-06-15 08:00:53Z bingmann $

#include <cppunit/extensions/HelperMacros.h>

#include "AnyType.h"

#include <stdlib.h>

class AnyTypeTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AnyTypeTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test1()
    {
	using namespace VGServer;

	AnyType t_bool (ATTRTYPE_BOOL);
	AnyType t_integer (ATTRTYPE_INTEGER);
	AnyType t_float (ATTRTYPE_FLOAT);
	AnyType t_string (ATTRTYPE_STRING);

	CPPUNIT_ASSERT( t_bool.setInteger(2) == true );
	CPPUNIT_ASSERT( t_bool.setInteger(1) == true );
	CPPUNIT_ASSERT( t_bool.getBoolean() == true );
	CPPUNIT_ASSERT( t_bool.getString() == "true" );

	CPPUNIT_ASSERT( t_bool.setDouble(0.3) );
	CPPUNIT_ASSERT( t_bool.getBoolean() == false );
	CPPUNIT_ASSERT( t_bool.getString() == "false" );

	CPPUNIT_ASSERT( t_bool.setString("t") );
	CPPUNIT_ASSERT( t_bool.getString() == "true" );


	CPPUNIT_ASSERT( t_integer.setInteger(42) );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );

	CPPUNIT_ASSERT( t_integer.setDouble(42.2) );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );

	CPPUNIT_ASSERT( t_integer.setString("42.2") == false );
	CPPUNIT_ASSERT( t_integer.setString("42") == true );
	CPPUNIT_ASSERT( t_integer.getBoolean() == true );
	CPPUNIT_ASSERT( t_integer.getInteger() == 42 );
	CPPUNIT_ASSERT( t_integer.getString() == "42" );


	CPPUNIT_ASSERT( t_float.setDouble(42) );
	CPPUNIT_ASSERT( t_float.getBoolean() == true );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getString() == "42.00" );

	CPPUNIT_ASSERT( t_float.setDouble(42.42) );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getString() == "42.42" );

	CPPUNIT_ASSERT( t_float.setString("42.42") );
	CPPUNIT_ASSERT( t_float.getInteger() == 42 );
	CPPUNIT_ASSERT( t_float.getString() == "42.42" );


	CPPUNIT_ASSERT( t_string.setInteger(20) );
	CPPUNIT_ASSERT_THROW( t_string.getBoolean(), ConversionException );
	CPPUNIT_ASSERT( t_string.getString() == "20" );

	CPPUNIT_ASSERT( t_string.setInteger(0) );
	CPPUNIT_ASSERT( t_string.getBoolean() == false );
	CPPUNIT_ASSERT( t_string.getString() == "0" );

	CPPUNIT_ASSERT( t_string.setInteger(-20) );
	CPPUNIT_ASSERT( t_string.getString() == "-20" );

	CPPUNIT_ASSERT( t_string.setStringQuoted("\"bla\\n\\\\\\\"h\"") );
	CPPUNIT_ASSERT( t_string.getString() == "bla\n\\\"h" );

	AnyType t_other = t_string;
	CPPUNIT_ASSERT( t_other.getType() == ATTRTYPE_STRING );
	CPPUNIT_ASSERT( t_other.getString() == t_string.getString() );
	CPPUNIT_ASSERT( t_other == t_string );

	t_other = t_integer;
	CPPUNIT_ASSERT( t_other.getType() == ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( t_other != t_string );
	CPPUNIT_ASSERT( t_other == t_integer );

	CPPUNIT_ASSERT( t_other.convertType(ATTRTYPE_STRING) );
	CPPUNIT_ASSERT( t_other.getInteger() == 42 );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnyTypeTest );
