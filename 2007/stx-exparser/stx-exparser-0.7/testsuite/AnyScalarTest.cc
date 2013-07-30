// $Id: AnyScalarTest.cc 59 2007-07-17 14:43:23Z tb $

/*
 * STX Expression Parser C++ Framework v0.7
 * Copyright (C) 2007 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cppunit/extensions/HelperMacros.h>

#include "ExpressionParser.h"

#include <stdlib.h>
#include <boost/lexical_cast.hpp>

using namespace stx;

class AnyScalarTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( AnyScalarTest );
    CPPUNIT_TEST(test_boolean);
    CPPUNIT_TEST(test_char);
    CPPUNIT_TEST(test_short);
    CPPUNIT_TEST(test_integer);
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    void test_boolean()
    {
	CPPUNIT_ASSERT( AnyScalar(bool(false)).getInteger() == 0 );
	CPPUNIT_ASSERT( AnyScalar(bool(true)).getInteger() == 1 );

	AnyScalar as(bool(false));

	CPPUNIT_ASSERT( as.setInteger(2) == true );
	CPPUNIT_ASSERT( as.setInteger(1) == true );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "true" );

	CPPUNIT_ASSERT( as.setDouble(0.3) );
	CPPUNIT_ASSERT( as.getBoolean() == false );
	CPPUNIT_ASSERT( as.getString() == "false" );

	CPPUNIT_ASSERT( as.setString("t") );
	CPPUNIT_ASSERT( as.getString() == "true" );

	CPPUNIT_ASSERT( as.getTypeString() == "bool" );
	CPPUNIT_ASSERT( as.getTypeLength() == 0 );
	CPPUNIT_ASSERT( as.getValueLength() == 0 );
	CPPUNIT_ASSERT( AnyScalar::stringToType("bool") == AnyScalar::ATTRTYPE_BOOL );
    }

    void test_char()
    {
	CPPUNIT_ASSERT( AnyScalar(char(42)).getInteger() == 42 );
	    
	AnyScalar as(char(42));

	CPPUNIT_ASSERT( as.setInteger(-32) == true );
	CPPUNIT_ASSERT( as.setInteger(10) == true );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "10" );

	CPPUNIT_ASSERT( as.setInteger(10010) == false );
	CPPUNIT_ASSERT( as.getString() == "127" );

	CPPUNIT_ASSERT( as.setDouble(0.3) );
	CPPUNIT_ASSERT( as.getBoolean() == false );
	CPPUNIT_ASSERT( as.getString() == "0" );

	CPPUNIT_ASSERT( as.setString("115") );
	CPPUNIT_ASSERT( as.getString() == "115" );
	CPPUNIT_ASSERT( as == AnyScalar(char(115)) );

	CPPUNIT_ASSERT( (as + AnyScalar(char(2))).getString() == "117" );

	CPPUNIT_ASSERT( as.getTypeString() == "char" );
	CPPUNIT_ASSERT( as.getTypeLength() == 1 );
	CPPUNIT_ASSERT( as.getValueLength() == 1 );
	CPPUNIT_ASSERT( AnyScalar::stringToType("char") == AnyScalar::ATTRTYPE_CHAR );
    }

    void test_short()
    {
	CPPUNIT_ASSERT( AnyScalar(short(442)).getInteger() == 442 );
	    
	AnyScalar as(short(442));

	CPPUNIT_ASSERT( as.setInteger(-432) == true );
	CPPUNIT_ASSERT( as.setInteger(310) == true );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "310" );

	CPPUNIT_ASSERT( as.setInteger(1340000010L) == false );
	CPPUNIT_ASSERT( as.getString() == "32767" );

	CPPUNIT_ASSERT( as.setDouble(300.3) );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "300" );

	CPPUNIT_ASSERT( as.setString("1115") );
	CPPUNIT_ASSERT( as.getString() == "1115" );
	CPPUNIT_ASSERT( as == AnyScalar(short(1115)) );

	CPPUNIT_ASSERT( (as + AnyScalar(char(2))).getString() == "1117" );

	CPPUNIT_ASSERT( as.getTypeString() == "short" );
	CPPUNIT_ASSERT( as.getTypeLength() == 2 );
	CPPUNIT_ASSERT( as.getValueLength() == 2 );
    }

    void test_integer()
    {
	CPPUNIT_ASSERT( AnyScalar(int(132442)).getInteger() == 132442 );
	    
	AnyScalar as(int(132442));

	CPPUNIT_ASSERT( as.setInteger(-132432) == true );
	CPPUNIT_ASSERT( as.setInteger(132310) == true );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "132310" );

	CPPUNIT_ASSERT( as.setLong(1340000000010LLU) == false );

	CPPUNIT_ASSERT( as.setDouble(322300.3) );
	CPPUNIT_ASSERT( as.getBoolean() == true );
	CPPUNIT_ASSERT( as.getString() == "322300" );
	CPPUNIT_ASSERT( as == AnyScalar(int(322300)) );

	CPPUNIT_ASSERT( as.setString("32221115") );
	CPPUNIT_ASSERT( as.getString() == "32221115" );

	CPPUNIT_ASSERT( (as + AnyScalar(char(2))).getString() == "32221117" );

	CPPUNIT_ASSERT( as.getTypeString() == "integer" );
	CPPUNIT_ASSERT( as.getTypeLength() == 4 );
	CPPUNIT_ASSERT( as.getValueLength() == 4 );
    }

    void test1()
    {
	AnyScalar t_integer (AnyScalar::ATTRTYPE_INTEGER);
	AnyScalar t_float (AnyScalar::ATTRTYPE_FLOAT);
	AnyScalar t_string (AnyScalar::ATTRTYPE_STRING);

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
	CPPUNIT_ASSERT( t_float.getDouble() == 42.0 );
	CPPUNIT_ASSERT( t_float.getString() == "42" );

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

	CPPUNIT_ASSERT( t_string.setStringQuoted("\"bla\\n\\\\\\\"h\\a\\b\\f\\r\\t\\v\\'x\"") );
	CPPUNIT_ASSERT( t_string.getString() == "bla\n\\\"h\a\b\f\r\t\v'x" );

	AnyScalar t_other = t_string;
	CPPUNIT_ASSERT( t_other.getType() == AnyScalar::ATTRTYPE_STRING );
	CPPUNIT_ASSERT( t_other.getString() == t_string.getString() );
	CPPUNIT_ASSERT( t_other == t_string );

	t_other = t_integer;
	CPPUNIT_ASSERT( t_other.getType() == AnyScalar::ATTRTYPE_INTEGER );
	CPPUNIT_ASSERT( t_other != t_string );
	CPPUNIT_ASSERT( t_other == t_integer );

	CPPUNIT_ASSERT( t_other.convertType(AnyScalar::ATTRTYPE_STRING) );
	CPPUNIT_ASSERT( t_other.getInteger() == 42 );

	// Arithmetic and Comparison Operators
	
	CPPUNIT_ASSERT( (AnyScalar(5) + AnyScalar(6)) == 11 );
	CPPUNIT_ASSERT( (AnyScalar(5) + AnyScalar(6)).getDouble() == 11.0 );

	CPPUNIT_ASSERT( (AnyScalar(5.0) + AnyScalar(4)).getString() == "9" );

	CPPUNIT_ASSERT( (AnyScalar("abc") + AnyScalar("def")) == "abcdef" );

	CPPUNIT_ASSERT( (AnyScalar("1.1") + AnyScalar(3.0)) == 4.1 );

	{
	    AnyScalar s;

	    CPPUNIT_ASSERT( s.setAutoString("abc").getType() == AnyScalar::ATTRTYPE_STRING );
	    CPPUNIT_ASSERT( s.setAutoString("0").getType() == AnyScalar::ATTRTYPE_INTEGER );
	    CPPUNIT_ASSERT( s.setAutoString("10045123").getTypeString() == "integer" );
	    CPPUNIT_ASSERT( s.setAutoString("10045123345343434").getType() == AnyScalar::ATTRTYPE_LONG );
	    CPPUNIT_ASSERT( s.setAutoString("-4554454").getType() == AnyScalar::ATTRTYPE_INTEGER );
	    CPPUNIT_ASSERT( s.setAutoString("-3402359829865").getTypeString() == "long" );
	    CPPUNIT_ASSERT( s.setAutoString("-3402359829865.3334").getTypeString() == "double" );
	    CPPUNIT_ASSERT( s.setAutoString("-34023598298abc65.3334").getTypeString() == "string" );
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnyScalarTest );
