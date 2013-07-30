// $Id: ExpressionParserTest.cc 59 2007-07-17 14:43:23Z tb $

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

class ExpressionParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ExpressionParserTest );
    CPPUNIT_TEST(test1);
    CPPUNIT_TEST_SUITE_END();

protected:

    inline stx::AnyScalar eval(const std::string &str)
    {
	stx::ParseTree pt = stx::parseExpression(str);

	// test reevaluation of the toString().
	{
	    std::string strexpr = pt.toString();
	    stx::ParseTree pt2 = stx::parseExpression(strexpr);

	    CPPUNIT_ASSERT( strexpr == pt2.toString() );
	}

	stx::BasicSymbolTable bst;
	bst.setVariable("a", 42);
	bst.setVariable("e", 2.7183);

	return pt.evaluate(bst);
    }

    void test1()
    {
	using namespace stx;

	CPPUNIT_ASSERT( eval("5 + 4.5") == 9.5 );

	CPPUNIT_ASSERT( eval(" \"abc\" + \"def\" ") == "abcdef" );

	CPPUNIT_ASSERT( eval("5 * 6 + 7 * 2 - 2") == 42 );

	CPPUNIT_ASSERT( eval("(integer)(5 * 1.5)") == 7 );

	CPPUNIT_ASSERT( eval("((integer)(5 * 1.5 >= 1)) + 5") == 6 );

	CPPUNIT_ASSERT( eval("(integer)(e)") == 2 );

	CPPUNIT_ASSERT( eval(" logn(exp(21)) + sqrt(pow(21,2)) == 42 ") == true );

	CPPUNIT_ASSERT( eval("10 > 5 AND 5.2 <= 42.2 OR 2 == 4") == true );

	CPPUNIT_ASSERT( eval("(a * 2 + 4) / 2 == 44") == true );

	CPPUNIT_ASSERT( eval("-5 + 4.5") == -0.5 );

	CPPUNIT_ASSERT( eval("-5 + 4.5 + (-a)") == -42.5 );

	CPPUNIT_ASSERT( eval(" \"newline \\n quoted \\\" a\\\\bc\" + \"def\" ") == "newline \n quoted \" a\\bcdef" );

	CPPUNIT_ASSERT( eval("(0 < 1) && (4 > 2) && (1 = 1) && (2 == 2) && (2 != 4) && (1 <= 1) && (1 >= 1) && (2 =< 3) && (4 => 1)") == true );

	CPPUNIT_ASSERT( eval("!(true AND false) || (NOT true OR FALSE)") == true );

	CPPUNIT_ASSERT_THROW( eval("5 +"), stx::ExpressionParserException );

	CPPUNIT_ASSERT_THROW( eval("5 + FUNCXYZ()"), stx::UnknownSymbolException );

	CPPUNIT_ASSERT_THROW( eval("5 + xyz"), stx::UnknownSymbolException );

	CPPUNIT_ASSERT_THROW( eval("5 + COS(2,2)"), stx::BadFunctionCallException );

	{
	    std::string xmlstr = stx::parseExpressionXML("((integer)(5 * 1.5 >= 1)) + 5");
	    CPPUNIT_ASSERT( xmlstr.size() == 1010 );
	}
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ExpressionParserTest );
