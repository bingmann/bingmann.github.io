// $Id: exprcalc.cc 59 2007-07-17 14:43:23Z tb $

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

/** \file exprcalc.cc
 * File containing the first example application:
 * A Simple Expression Calculator.
 * See the doxygen documentation for a detailed walk-through of the code.
 */

// Simple Expression Calculator Example

#include <iostream>
 
#include "ExpressionParser.h"

int main(int argc, char *argv[])
{
    // collect expression by joining all input arguments
    std::string args;
    for(int i = 1; i < argc; i++) {
	if (!args.empty()) args += " ";
	args += argv[i];
    }

    std::cout << "input string: " << args << "\n";

    // parse expression into a parse tree
    stx::ParseTree pt;
    try
    {
	pt = stx::parseExpression(args);
	std::cout << "parsed: " << pt.toString() << "\n";
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cout << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    // evaluate the expression with a very simple symbol table
    stx::BasicSymbolTable bst;
    bst.setVariable("x", 42);

    try
    {
	stx::AnyScalar val = pt.evaluate(bst);

	std::cout << "evaluated: " << val << "\n";
    }
    catch (stx::UnknownSymbolException &e)
    {
	std::cout << "UnknownSymbolException: " << e.what() << "\n";
	return 0;
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cout << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    return 0;
}
