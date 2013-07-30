// $Id: csvfilter.cc 59 2007-07-17 14:43:23Z tb $

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

/** \file csvfilter.cc
 * File containing the second example application:
 * A CSV Parser and Filter using the Expression Parser 
 * See the doxygen documentation for a detailed walk-through of the code.
 */

// CSV Parser and Filter using the Expression Parser
 
#include "ExpressionParser.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

// use this as the delimiter. this can be changed to ';' or ',' if needed
const char delimiter = '\t';

// read one line from instream and split it into tab (or otherwise) delimited
// columns. returns the number of columns read, 0 if eof.
unsigned int read_csvline(std::istream &instream,
			  std::vector<std::string> &columns)
{
    columns.clear();

    // read one line from the input stream
    std::string line;
    if (!std::getline(instream, line, '\n').good()) {
	return 0;
    }

    // parse line into tab separated columns, start with inital column
    columns.push_back("");

    for (std::string::const_iterator si = line.begin();
	 si != line.end(); ++si)
    {
	if (*si == delimiter)
	    columns.push_back("");
	else // add non-delimiter to last column
	    columns.back() += *si;
    }

    return columns.size();
}

// subclass stx::BasicSymbolTable and return variable values from the current
// csv row. the variable names are defined by the map containing the column
// header.
class CSVRowSymbolTable : public stx::BasicSymbolTable
{
public:
    // maps the column variable name to the vector index
    const std::map<std::string, unsigned int> &headersmap;

    // refernce to the reused data row vector.
    const std::vector<std::string> &datacolumns;

    CSVRowSymbolTable(const std::map<std::string, unsigned int> &_headersmap,
		      const std::vector<std::string> &_datacolumns)
	: stx::BasicSymbolTable(),
	  headersmap(_headersmap),
	  datacolumns(_datacolumns)
    {
    }

    virtual stx::AnyScalar lookupVariable(const std::string &varname) const
    {
	// look if the variable name is defined by the CSV file
	std::map<std::string, unsigned int>::const_iterator
	    varfind = headersmap.find(varname);

	if (varfind == headersmap.end()) {
	    // if not, let BasicSymbolTable check if it knows it
	    return stx::BasicSymbolTable::lookupVariable(varname);
	}

	// return the variable value from the current vector. convert it to a
	// stx::AnyScalar but use the automatic type recognition for input
	// strings.
	if(varfind->second < datacolumns.size())
	{
	    return stx::AnyScalar().setAutoString( datacolumns[varfind->second] );
	}
	else
	{
	    return "";	// happens when a data row has too few delimited
			// fields.
	}
    }
};

int main(int argc, char *argv[])
{
    // collect expression by joining all remaining input arguments
    std::string args;
    for(int i = 1; i < argc; i++) {
	if (!args.empty()) args += " ";
	args += argv[i];
    }

    std::cerr << "Expression string: " << args << "\n";

    // parse expression into a parse tree
    stx::ParseTree pt;
    try
    {
	pt = stx::parseExpression(args);
	std::cerr << "Parsed expression: " << pt.toString() << "\n";
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cerr << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    // read first line of CSV input as column headers
    std::cerr << "Reading CSV column headers from input\n";
    std::vector<std::string> headers;

    if (read_csvline(std::cin, headers) == 0) {
	std::cerr << "Error read column headers: no input\n";
	return 0;
    }

    std::cerr << "Read " << headers.size() << " column headers.\n";

    // create a header column lookup map for CSVRowSymbolTable and output the
    // column header line to std::cout
    std::map<std::string, unsigned int> headersmap;
    for(unsigned int headnum = 0; headnum < headers.size(); ++headnum)
    {
	headersmap[ headers[headnum] ] = headnum;

	if (headnum != 0) std::cout << delimiter;
	std::cout << headers[headnum];
    }
    std::cout << "\n";

    // iterate over the data lines of the CSV input
    unsigned int linesprocessed = 0, linesskipped = 0;
    std::vector<std::string> datacolumns;
    CSVRowSymbolTable csvsymboltable(headersmap, datacolumns);

    while( read_csvline(std::cin, datacolumns) > 0 )
    {
        // evaluate the expression for each row using the headers/datacolumns
        // as variables
	try
	{
	    linesprocessed++;
	    stx::AnyScalar val = pt.evaluate( csvsymboltable );

	    if (val.isBooleanType())
	    {
		if (!val.getBoolean()) {
		    linesskipped++;
		    continue;
		}
	    }
	    else {
		std::cerr << "evaluated: " << val << "\n";
	    }

	    // output this data row to std::cout
	    for(std::vector<std::string>::const_iterator
		    coliter = datacolumns.begin();
		coliter != datacolumns.end(); ++coliter)
	    {
		if (coliter != datacolumns.begin()) std::cout << delimiter;
		std::cout << *coliter;
	    }
	    std::cout << "\n";
	}
	catch (stx::UnknownSymbolException &e)
	{
	    std::cerr << "evaluated: UnknownSymbolException: " << e.what() << "\n";
	}
	catch (stx::ExpressionParserException &e)
	{
	    std::cerr << "evaluated: ExpressionParserException: " << e.what() << "\n";
	}
    }
    std::cerr << "Processed " << linesprocessed << " lines, "
	      << "copied " << (linesprocessed - linesskipped) << " and "
	      << "skipped " << linesskipped << " lines" << "\n";
}
