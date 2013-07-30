// $Id: csvtool.cc 59 2007-07-17 14:43:23Z tb $

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

/** \file csvtool.cc
 * File containing another example application:
 * A CSV Parser, Filter and Sorter using the Expression Parser
 * See the doxygen documentation for a short specification.
 */

// Enhanced CSV Parser and Filter using the Expression Parser
 
#include "ExpressionParser.h"
#include "strnatcmp.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <boost/lexical_cast.hpp>

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

// std::sort order relation functional object
struct DataRecordSortRelation
{
    // the column index to sort by
    unsigned int sortcol;

    // sort ascending or descending
    bool	descending;

    inline DataRecordSortRelation(unsigned int _sortcol, bool _descending = 0)
	: sortcol(_sortcol), descending(_descending)
    {
    }

    // calls the strnatcasecmp on the given column's text or "" if the column
    // does not exist. sorts in "natural" sort order means numbers and text are
    // ordered correctly.
    inline bool operator()(const std::vector<std::string> &recordA,
			   const std::vector<std::string> &recordB) const
    {
	if (!descending) {
	    return strnatcasecmp(sortcol < recordA.size() ? recordA[sortcol].c_str() : "",
				 sortcol < recordB.size() ? recordB[sortcol].c_str() : "") < 0;
	}
	else {
	    return strnatcasecmp(sortcol < recordA.size() ? recordA[sortcol].c_str() : "",
				 sortcol < recordB.size() ? recordB[sortcol].c_str() : "") > 0;
	}
    }
};

// trim function from my weblog.
static inline std::string string_trim(const std::string& str)
{
    std::string::size_type pos1 = str.find_first_not_of(' ');
    if (pos1 == std::string::npos) return std::string();

    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 == std::string::npos) return std::string();

    return str.substr(pos1 == std::string::npos ? 0 : pos1,
                      pos2 == std::string::npos ? (str.length() - 1) : (pos2 - pos1 + 1));
}

int main(int argc, char *argv[])
{
    // get progarm argment or reasonable defaults
    if (argc < 2) {
	std::cerr << "Usage: " << argv[0] << " <csv-filename> [filter expression] [sort-column] [offset] [limit]" << "\n";
	return 0;
    }

    std::string csvfilename = argv[1];
    std::string exprstring = (argc >= 3) ? string_trim(argv[2]) : "";
    std::string sortcolumn = (argc >= 4) ? string_trim(argv[3]) : "";
    std::string offsetstring = (argc >= 5) ? string_trim(argv[4]) : "";
    std::string limitstring = (argc >= 6) ? string_trim(argv[5]) : "";

    // parse expression into a parse tree
    stx::ParseTree pt;
    try
    {
	if (exprstring.size()) {
	    pt = stx::parseExpression(exprstring);
	}
    }
    catch (stx::ExpressionParserException &e)
    {
	std::cerr << "ExpressionParserException: " << e.what() << "\n";
	return 0;
    }

    // open the CSV file stream, either stdin or the given filename
    std::ifstream csvfilestream;
    if (csvfilename != "-")
    {
	csvfilestream.open(csvfilename.c_str());
	if (!csvfilestream) {
	    std::cerr << "Error opening CSV file " << csvfilename << "\n";
	    return 0;
	}
    }

    std::istream& csvfile = (csvfilename == "-") ? std::cin : csvfilestream;

    // read first line of CSV input as column headers
    std::vector<std::string> headers;

    if (read_csvline(csvfile, headers) == 0) {
	std::cerr << "Error read column headers: no input\n";
	return 0;
    }

    // create a header column lookup map for CSVRowSymbolTable 
    std::map<std::string, unsigned int> headersmap;
    for(unsigned int headnum = 0; headnum < headers.size(); ++headnum)
    {
	headersmap[ headers[headnum] ] = headnum;
    }

    // iterate over the data lines of the CSV input and save matching data rows
    // into "datarecords"
    unsigned int linesprocessed = 0;
    bool addedEvalResult = false;

    std::vector<std::string> datacolumns;	// current row
    CSVRowSymbolTable csvsymboltable(headersmap, datacolumns);

    // huge table containing copied rows.
    std::vector< std::vector<std::string> > datarecords;

    while( read_csvline(csvfile, datacolumns) > 0 )
    {
        // evaluate the expression for each row using the headers/datacolumns
        // as variables
	try
	{
	    linesprocessed++;
	    if (!pt.isEmpty())
	    {
		stx::AnyScalar val = pt.evaluate( csvsymboltable );

		if (val.isBooleanType())
		{
		    if (!val.getBoolean()) continue;
		}
		else
		{
		    // if calculation results in non-boolean value, then save
		    // that value into a column "EvalResult"
		    if (!addedEvalResult) {
			headers.push_back("EvalResult");
			addedEvalResult = true;
		    }

		    while( datacolumns.size() + 1 < headers.size() )
			datacolumns.push_back("");
		    datacolumns.push_back(val.getString());
		}
	    }
	}
	catch (stx::ExpressionParserException &e)
	{
	    // save exception text into column "EvalResult"
	    if (!addedEvalResult) {
		headers.push_back("EvalResult");
		addedEvalResult = true;
	    }
	    // add calculation result as last column
	    while( datacolumns.size() + 1 < headers.size() )
		datacolumns.push_back("");

	    datacolumns.push_back(std::string("Exception: ") + e.what());
	}

	datarecords.push_back( datacolumns );
    }

    // add "EvalResult" to headers map to allow sorting by it.
    if (addedEvalResult) {
	headersmap[ headers[headers.size() - 1] ] = headers.size() - 1;
    }

    // sort the result table if required.
    if (sortcolumn.size())
    {
	std::map<std::string, unsigned int>::const_iterator
	    colfind = headersmap.find(sortcolumn);

	if (colfind != headersmap.end())
	{
	    // sort ascending
	    std::sort(datarecords.begin(), datarecords.end(),
		      DataRecordSortRelation(colfind->second));
	}
	else 
	{
	    // the the sort column is !header it is sorted descending.
	    if (sortcolumn[0] == '!')
	    {
		sortcolumn.erase(0, 1);
		
		colfind = headersmap.find(sortcolumn);
		if (colfind != headersmap.end())
		{
		    std::sort(datarecords.begin(), datarecords.end(),
			      DataRecordSortRelation(colfind->second, 1));
		}
		else
		{
		    std::cerr << "Bad sort column: " << sortcolumn << " could not be found.\n";
		    return 0;
		}
	    }
	    else
	    {
		std::cerr << "Bad sort column: " << sortcolumn << " could not be found.\n";
		return 0;
	    }
	}
    }

    // determine offset and limit of the outputted data rows.

    unsigned int offset = 0;
    unsigned int limit = datarecords.size();

    if (offsetstring.size())
    {
	try {
	    offset = boost::lexical_cast<unsigned int>(offsetstring);
	}
	catch (boost::bad_lexical_cast &e) {
	    std::cerr << "Bad number in offset: not an integer.\n";
	    return 0;
	}
    }

    if (limitstring.size())
    {
	try {
	    limit = boost::lexical_cast<unsigned int>(limitstring);
	}
	catch (boost::bad_lexical_cast &e) {
	    std::cerr << "Bad number in limit: not an integer.\n";
	    return 0;
	}
    }

    // write a processing summary to stderr
    std::cerr << "Processed " << linesprocessed << " lines, "
	      << "copied " << datarecords.size() << " and "
	      << "skipped " << (linesprocessed - datarecords.size()) << " lines" << "\n";


    // write column headers to stdout
    for(std::vector<std::string>::const_iterator coliter = headers.begin();
	coliter != headers.end(); ++coliter)
    {
	if (coliter != headers.begin()) std::cout << delimiter;
	std::cout << *coliter;
    }
    std::cout << "\n";

    // output data rows from "offset" to "offset+limit"

    for(unsigned int current = offset;
	current < offset + limit && current < datarecords.size();
	++current)
    {
	std::vector<std::string> &currrecord = datarecords[current];

	// output this data row to std::cout
	for(std::vector<std::string>::const_iterator coliter = currrecord.begin();
	    coliter != currrecord.end(); ++coliter)
	{
	    if (coliter != currrecord.begin()) std::cout << delimiter;
	    std::cout << *coliter;
	}
	std::cout << "\n";
    }
}
