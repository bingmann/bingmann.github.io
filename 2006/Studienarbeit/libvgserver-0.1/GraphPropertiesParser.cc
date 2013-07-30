// $Id: GraphPropertiesParser.cc 165 2006-05-29 16:39:53Z bingmann $

// #define BOOST_SPIRIT_DEBUG

#include <boost/spirit/core.hpp>

#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/utility/distinct.hpp>

#include <boost/spirit/phoenix.hpp>

#include "GraphProperties.h"
#include "AttributeProperties.h"
#include "GraphTypes.h"

#include <string>

namespace VGServer {

using namespace boost::spirit;
using namespace phoenix;

/** GraphPropertiesParser is an boost::spirit parser which reads a very simple
 * attribute specification config file format. It puts the read info into the
 * given GraphProperties reference.
 *
 * The config format looks like this:

directed 

vertexattr (
  x             integer         varying,
  y             integer         varying,
  name1		integer		default 42,
  name2		string		default "abc",
  someothername	bool		default true,
  more          string
)

edgeattr (
  eattr1	char		default 5,
)

 * Not all of the values are fulled supported yet. This was more an exercise to
 * check spirit out. Spirit may be the parser generator of choice for the
 * filter expression received from the client.
*/

static member_function_ptr<void, AttributeProperties, attrtype_t> phx_AnyType_resetType = &AttributeProperties::resetType;
static member_function_ptr<bool, AttributeProperties, int> phx_AnyType_setInteger = &AttributeProperties::setInteger;
static member_function_ptr<bool, AttributeProperties, double> phx_AnyType_setDouble = &AttributeProperties::setDouble;
static member_function_ptr<bool, AttributeProperties, const std::string&> phx_AnyType_setString = &AttributeProperties::setString;
static member_function_ptr<bool, AttributeProperties, const std::string&> phx_AnyType_setStringQuoted = &AttributeProperties::setStringQuoted;

const distinct_parser<> key_p("0-9a-zA-Z_");

struct GraphPropertiesParser : public grammar<GraphPropertiesParser>
{
    template <typename ScannerT>
    struct definition
    {
        definition(GraphPropertiesParser const& self)
        {
	    // require the three componds to be in order!
            r_start = r_directed || r_vertexattr || r_edgeattr;

	    // first a single keyword: directed or undirected.
	    r_directed = key_p("directed")[var(self.gp.directed) = 1]
		       | key_p("undirected")[var(self.gp.directed) = 0];
	    
	    // then the vertexattr ( .. ) list
	    r_vertexattr = key_p("vertexattr") >> ch_p('(') >> r_attr[push_back_a(self.gp.vertexattr, ap)] >> r_vertexattrlist;

	    r_vertexattrlist = ch_p(')')
                             | ch_p(',') >> r_attr[push_back_a(self.gp.vertexattr, ap)] >> r_vertexattrlist;

	    r_edgeattr = key_p("edgeattr") >> ch_p('(') >> r_attr[push_back_a(self.gp.edgeattr, ap)] >> r_edgeattrlist;

	    r_edgeattrlist = ch_p(')')
		           | ch_p(',') >> r_attr[push_back_a(self.gp.edgeattr, ap)] >> r_edgeattrlist;

	    // an attribute consist of it's name and the type defintion
	    r_attr = lexeme_d[+graph_p][var(ap.name) = construct_<std::string>(arg1, arg2)] >> r_attrtype;

	    // all types are listed here with the action to set the type on the temporary attribute.
	    // ! means that the expression is _optional_
	    r_attrtype =
		  (key_p("bool")[phx_AnyType_resetType(var(ap), ATTRTYPE_BOOL)] >> !r_attrdefbool)

		| (key_p("char")[phx_AnyType_resetType(var(ap), ATTRTYPE_CHAR)] >> !r_attrdefint)
		| (key_p("short")[phx_AnyType_resetType(var(ap), ATTRTYPE_SHORT)] >> !r_attrdefint)
		| (key_p("integer")[phx_AnyType_resetType(var(ap), ATTRTYPE_INTEGER)] >> !r_attrdefint)
		| (key_p("long")[phx_AnyType_resetType(var(ap), ATTRTYPE_LONG)] >> !r_attrdefint)

		| (key_p("byte")[phx_AnyType_resetType(var(ap), ATTRTYPE_BYTE)] >> !r_attrdefuint)
		| (key_p("word")[phx_AnyType_resetType(var(ap), ATTRTYPE_WORD)] >> !r_attrdefuint)
		| (key_p("dword")[phx_AnyType_resetType(var(ap), ATTRTYPE_DWORD)] >> !r_attrdefuint)
		| (key_p("qword")[phx_AnyType_resetType(var(ap), ATTRTYPE_QWORD)] >> !r_attrdefuint)

		| (key_p("float")[phx_AnyType_resetType(var(ap), ATTRTYPE_FLOAT)] >> !r_attrdefreal)
		| (key_p("double")[phx_AnyType_resetType(var(ap), ATTRTYPE_DOUBLE)] >> !r_attrdefreal)

		| (key_p("string")[phx_AnyType_resetType(var(ap), ATTRTYPE_STRING)] >> !r_attrdefstring)
		| (key_p("longstring")[phx_AnyType_resetType(var(ap), ATTRTYPE_LONGSTRING)] >> !r_attrdefstring)
		;

	    // the following are parsers for the default ... values, which can be of different types.

	    r_attrdefbool = 
		( key_p("default") >>
		  (key_p("true") | key_p("false") | key_p("yes") | key_p("no"))
		  [phx_AnyType_setString(var(ap), construct_<std::string>(arg1, arg2))] 
		  |
		  key_p("varying")[var(ap.varying) = true]
		    );

	    r_attrdefint = 
		( key_p("default") >> int_p[phx_AnyType_setInteger(var(ap), arg1)]
		  |
		  key_p("varying")[var(ap.varying) = true]
		    );

	    r_attrdefuint =
		( key_p("default") >> uint_p[phx_AnyType_setInteger(var(ap), arg1)]
		  |
		  key_p("varying")[var(ap.varying) = true]
		    );

	    r_attrdefreal =
		( key_p("default") >> real_p[phx_AnyType_setDouble(var(ap), arg1)]
		  |
		  key_p("varying")[var(ap.varying) = true]
		    );

	    // a string default value must be enclosed in "
	    r_attrdefstring = key_p("default") >>
		confix_p('"', (*c_escape_ch_p), '"')[phx_AnyType_setStringQuoted(var(ap), construct_<std::string>(arg1, arg2))];

#ifdef BOOST_SPIRIT_DEBUG
	    BOOST_SPIRIT_DEBUG_RULE(r_attr);
	    BOOST_SPIRIT_DEBUG_RULE(r_attrtype);

	    BOOST_SPIRIT_DEBUG_RULE(r_attrdefbool);
	    BOOST_SPIRIT_DEBUG_RULE(r_attrdefint);
	    BOOST_SPIRIT_DEBUG_RULE(r_attrdefuint);
	    BOOST_SPIRIT_DEBUG_RULE(r_attrdefreal);
	    BOOST_SPIRIT_DEBUG_RULE(r_attrdefstring);
#endif
        }

	rule<ScannerT> r_start, r_directed,
                       r_vertexattr, r_vertexattrlist,
                       r_edgeattr, r_edgeattrlist,
                       r_attr, r_attrtype,
                       r_attrdefbool, r_attrdefint, r_attrdefuint, r_attrdefreal, r_attrdefstring;

        rule<ScannerT> const& start() const
	{ return r_start; }

	// temporary AttributeProperties object
	class AttributeProperties ap;
    };

    GraphPropertiesParser(class GraphProperties &_gp)
	: gp(_gp)
    { }

    // reference to object which is filled with the parsed data
    class GraphProperties &gp;
};

/// This is the skip grammar. This grammar defines the language that
/// the parser will use to skip white space and others
struct GraphPropertiesSkipParser : public grammar<GraphPropertiesSkipParser>
{
    template<typename ScannerT>
    struct definition
    {
        definition(const GraphPropertiesSkipParser& /*self*/)
        {
            skip
                = space_p
                | comment_p("//")
		| comment_p("/*", "*/")
                ;
            
            #ifdef BOOST_SPIRIT_DEBUG
            BOOST_SPIRIT_DEBUG_RULE(skip);
            #endif
        }
        
	rule<ScannerT> skip;

        const rule<ScannerT>& start() const {
            return skip;
        }
    };
};

bool GraphProperties::parseConfigString(const std::string &input)
{
    GraphPropertiesParser gpp(*this);
    GraphPropertiesSkipParser gpsp;

    return boost::spirit::parse(input.c_str(), gpp, gpsp).full;
}

} // namespace VGServer


