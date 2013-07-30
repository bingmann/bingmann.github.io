// $Id: AttributeParser.cc 238 2006-06-29 17:28:25Z bingmann $

// #define BOOST_SPIRIT_DEBUG

#include "GraphContainer.h"

#include "AttributeParser.h"
#include "AttributeProperties.h"
#include "GraphProperties.h"
#include "GraphPort.h"

#include "AttributeBlob_impl.h"

#include "ByteOutBuffer.h"

#include <boost/spirit/core.hpp>

#include <boost/spirit/tree/ast.hpp>

#include <boost/spirit/utility/lists.hpp>
#include <boost/spirit/utility/distinct.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/grammar_def.hpp> 

#include <boost/spirit/dynamic/if.hpp>

#ifdef BOOST_SPIRIT_DEBUG
#include <boost/spirit/tree/tree_to_xml.hpp>
#endif

#include <iostream>
#include <sstream>

namespace VGServer {

/// namespace to enclose the spirit parser grammar and hidden selector or
/// filter node classes
namespace AttributeParser {

using namespace boost::spirit;

// this enum specifies ids for the parse tree nodes created for each rule.
enum parser_ids
{
    integer_const_id = 1,
    double_const_id,
    string_const_id,
    constant_id,

    function_call_id,
    function_identifier_id,

    attrname_id,

    atom_expr_id,

    unary_expr_id,
    mul_expr_id,
    add_expr_id,

    cast_expr_id,
    cast_spec_id,

    comp_expr_id,
    and_expr_id,
    or_expr_id,

    expr_id,

    attrlist_star_id,
    attrlist_exprlist_id,
    attrlist_id,

    filter_expr_id,
};

/// keyword parser
distinct_parser<> keyword_p("a-zA-Z0-9_");

/// the boost::spirit select attribute grammar
struct AttributeGrammar : public grammar<AttributeGrammar>
{
    template <typename ScannerT>
    struct definition : public grammar_def<rule<ScannerT, parser_context<>, parser_tag<attrlist_id> >,
					   rule<ScannerT, parser_context<>, parser_tag<filter_expr_id> > >
    {
	definition(AttributeGrammar const& /*self*/)
	{
	    // *** Constants

	    constant
		= double_const
		| integer_const
		| string_const
		;
	    
	    integer_const
		= int_p
		;

	    double_const
		= strict_real_p
		;

            string_const
                = lexeme_d[
		    token_node_d[ '"' >> *(c_escape_ch_p - '"') >> '"' ]
                    ]
                ;

	    // *** Function call and function identifier

            function_call
                = function_identifier >> discard_node_d[ ch_p('(') ] 
				      >> infix_node_d[ !list_p(expr, ch_p(',')) ]
				      >> discard_node_d[ ch_p(')') ]
                ;

            function_identifier
                = lexeme_d[ 
		    token_node_d[ alpha_p >> *(alnum_p | ch_p('_')) ]
                    ]
                ;

	    // *** Attribute names

            attrname
                = lexeme_d[ 
		    token_node_d[ alpha_p >> *(alnum_p | ch_p('_'))
				  >> !( ch_p('.') >> alpha_p >> *(alnum_p | ch_p('_')) ) ]
                    ]
                ;
	    // *** Valid Expressions, from small to large

            atom_expr
                = constant
                | inner_node_d[ ch_p('(') >> expr >> ch_p(')') ]
                // TODO: we want function calls? | function_call
		| attrname
                ;

            unary_expr
                = !( root_node_d[ch_p('+') | ch_p('-') | ch_p('!') | str_p("not")] )
		    >> atom_expr
                ;

            mul_expr
                = unary_expr >>
		  *( root_node_d[ch_p('*')] >> unary_expr 
		   | root_node_d[ch_p('/')] >> unary_expr )
                ;

	    add_expr
		= mul_expr >>
		  *( root_node_d[ch_p('+')] >> mul_expr
		   | root_node_d[ch_p('-')] >> mul_expr )
		;

	    cast_spec
		= as_lower_d[
		    discard_node_d[ keyword_p("cast") ] >>
		    (
			keyword_p("bool") |
			keyword_p("char") | keyword_p("short") | keyword_p("integer") | keyword_p("long") |
			keyword_p("byte") | keyword_p("word") | keyword_p("dword") | keyword_p("qword") |
			keyword_p("float") | keyword_p("double") |
			keyword_p("string") | keyword_p("longstring")
		     )
		    ]
		;

            cast_expr
		= add_expr >> root_node_d[ !cast_spec ]
		;

	    comp_expr
		= cast_expr >>
		*( root_node_d[str_p("==")] >> cast_expr 
		 | root_node_d[ch_p('=')] >> cast_expr
		 | root_node_d[str_p("!=")] >> cast_expr
		 | root_node_d[ch_p('<')] >> cast_expr
		 | root_node_d[ch_p('>')] >> cast_expr
		 | root_node_d[str_p("<=")] >> cast_expr
		 | root_node_d[str_p(">=")] >> cast_expr
		 | root_node_d[str_p("=<")] >> cast_expr
		 | root_node_d[str_p("=>")] >> cast_expr
		 )
		;

	    and_expr
		= comp_expr >>
		*( root_node_d[ as_lower_d[ str_p("and")] ] >> comp_expr
		 | root_node_d[ as_lower_d[ str_p("&&")] ] >> comp_expr
		 )
		;

	    or_expr
		= and_expr >>
		*( root_node_d[ as_lower_d[ str_p("or")] ] >> and_expr
		 | root_node_d[ as_lower_d[ str_p("||")] ] >> and_expr
		 )
		;

	    expr
		= or_expr
		;

	    // *** Rules for the attribute selector list parser
	    
	    attrlist_star
		= ch_p('*')
		;

	    attrlist_exprlist
		= infix_node_d[ !list_p(expr, ch_p(',')) ]
		;

	    attrlist
		= attrlist_star
		| attrlist_exprlist
		;

	    // *** Rules for the filter parser

	    filter_expr
		= !( root_node_d[ keyword_p("vertex:")
				| keyword_p("vertices:")
				| keyword_p("vertics:")
				| keyword_p("v:")
				| keyword_p("edge:")
				| keyword_p("edges:")
				| keyword_p("e:") ]
		     >> expr )
		;

	    // this is a special spirit feature to declare multiple grammar entry points
	    this->start_parsers(attrlist, filter_expr); 

#ifdef BOOST_SPIRIT_DEBUG
	    BOOST_SPIRIT_DEBUG_RULE(constant);

	    BOOST_SPIRIT_DEBUG_RULE(integer_const);
	    BOOST_SPIRIT_DEBUG_RULE(double_const);
	    BOOST_SPIRIT_DEBUG_RULE(string_const);

	    BOOST_SPIRIT_DEBUG_RULE(function_call);
	    BOOST_SPIRIT_DEBUG_RULE(function_identifier);
	    
	    BOOST_SPIRIT_DEBUG_RULE(attrname);

	    BOOST_SPIRIT_DEBUG_RULE(atom_expr);

	    BOOST_SPIRIT_DEBUG_RULE(unary_expr);
	    BOOST_SPIRIT_DEBUG_RULE(mul_expr);
	    BOOST_SPIRIT_DEBUG_RULE(add_expr);

	    BOOST_SPIRIT_DEBUG_RULE(cast_spec);
	    BOOST_SPIRIT_DEBUG_RULE(cast_expr);

	    BOOST_SPIRIT_DEBUG_RULE(comp_expr);
	    BOOST_SPIRIT_DEBUG_RULE(and_expr);
	    BOOST_SPIRIT_DEBUG_RULE(or_expr);

	    BOOST_SPIRIT_DEBUG_RULE(expr);

	    BOOST_SPIRIT_DEBUG_RULE(attrlist_star);
	    BOOST_SPIRIT_DEBUG_RULE(attrlist_exprlist);
	    BOOST_SPIRIT_DEBUG_RULE(attrlist);

	    BOOST_SPIRIT_DEBUG_RULE(filter_expr);
#endif
	}

        rule<ScannerT, parser_context<>, parser_tag<constant_id> > 		constant;

        rule<ScannerT, parser_context<>, parser_tag<integer_const_id> > 	integer_const;
        rule<ScannerT, parser_context<>, parser_tag<double_const_id> > 		double_const;
        rule<ScannerT, parser_context<>, parser_tag<string_const_id> > 		string_const;

        rule<ScannerT, parser_context<>, parser_tag<function_call_id> > 	function_call;
        rule<ScannerT, parser_context<>, parser_tag<function_identifier_id> > 	function_identifier;

        rule<ScannerT, parser_context<>, parser_tag<attrname_id> > 		attrname;

        rule<ScannerT, parser_context<>, parser_tag<atom_expr_id> > 		atom_expr;

        rule<ScannerT, parser_context<>, parser_tag<unary_expr_id> > 		unary_expr;
        rule<ScannerT, parser_context<>, parser_tag<mul_expr_id> > 		mul_expr;
        rule<ScannerT, parser_context<>, parser_tag<add_expr_id> > 		add_expr;

        rule<ScannerT, parser_context<>, parser_tag<cast_spec_id> >        	cast_spec;
        rule<ScannerT, parser_context<>, parser_tag<cast_expr_id> >        	cast_expr;

        rule<ScannerT, parser_context<>, parser_tag<comp_expr_id> >        	comp_expr;
        rule<ScannerT, parser_context<>, parser_tag<and_expr_id> >        	and_expr;
        rule<ScannerT, parser_context<>, parser_tag<or_expr_id> >        	or_expr;

        rule<ScannerT, parser_context<>, parser_tag<expr_id> >        		expr;

        rule<ScannerT, parser_context<>, parser_tag<attrlist_star_id> >    	attrlist_star;
        rule<ScannerT, parser_context<>, parser_tag<attrlist_exprlist_id> >    	attrlist_exprlist;
        rule<ScannerT, parser_context<>, parser_tag<attrlist_id> >    		attrlist;

        rule<ScannerT, parser_context<>, parser_tag<filter_expr_id> >       	filter_expr;
    };
};

// *** Classes representing the nodes in the resulting parse tree, these need
// *** not be publicly available via the header file.

/// Constant value nodes of the parse tree. This class holds any of the three
/// constant types in the enclosed AnyType object.
class PNConstant : public ParseNode
{
private:
    /// the constant parsed value.
    class AnyType	val;

public:
    /// assignment from the string received from the parser.
    PNConstant(attrtype_t type, std::string strvalue)
	: ParseNode(), val(type)
    {
	if (type == ATTRTYPE_STRING)
	    val.setStringQuoted(strvalue);
	else
	    val.setString(strvalue);
    }

    /// constructor for folded constant values.
    PNConstant(const AnyType &_val)
	: val(_val)
    {
    }

    /// easiest getValue: return the constant.
    virtual bool getValue(AnyType &dest, const class GraphContainer& , const class Changelist& ,
			  vertex_or_edge_t , unsigned int , unsigned int ) const
    {
	dest = val;
	return true;
    }

    /// returns true, because value is constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (dest) *dest = val;
	return true;
    }

    /// string representation of the constant AnyType value.
    virtual std::string toString() const
    {
	return val.getString();
    }
};

#define WITH_CHANGELIST

/// Parse tree node representing the vertex or edge attribute value
/// place-holder. It is then filled when parameterized by a Vertex or Edge
/// object.
class PNAttributeName : public ParseNode
{
private:
    /// string name of the attribute
    std::string		attrname;

    /// attribute id
    unsigned int	attrid;

    /// attribute type and default value
    AnyType		attrval;

public:
    /// constructor from the string received from the parser.
    PNAttributeName(std::string _attrname, const AttributePropertiesList &attrlist, vertex_or_edge_t voe)
	: ParseNode(), attrname(_attrname), attrval(ATTRTYPE_INVALID)
    {
	if (voe == VE_VERTEX and attrname == "id") {
	    attrid = UINT_MAX;
	    attrval = 0U;
	    return;
	}

	int _attrid = attrlist.lookupAttributeName(attrname);
	if (_attrid < 0)
	    throw(AttributeParseException("Unknown attribute name " + attrname + ". Remember they are case-sensitive."));

	attrid = _attrid;
	attrval = attrlist[attrid];
    }

    /// returns the value of the specified attribute in the current vertex or
    /// edge attribute value chain.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	if (not wantedge) // vertex attribute selection
	{
	    if (attrid == UINT_MAX) {
		dest = vid1;
		return true;
	    }

	    assert( attrid < gc.getProperties().vertexattr.size() );
	    assert( gc.getProperties().vertexattr[attrid].name == attrname );

#ifdef WITH_CHANGELIST
 	    VertexRef vr = gc.getVertex(vid1, cl);
	    if (!vr.valid())
	    {
		dest = gc.getProperties().vertexattr[attrid];
		return false;
	    }
	    else
	    {
		dest = vr.getAttr(attrid);
	    }
#else
	    if (!gc.existVertex(vid1))
		return false;

	    const GraphData::Vertex* vp = static_cast<const GraphData&>(gc).getVertex(vid1);
	    
	    dest = vp->getAttr(attrid, gc);
#endif
	}
	else
	{
	    // edge attribute selector

	    assert( attrid < gc.getProperties().edgeattr.size() );
	    assert( gc.getProperties().edgeattr[attrid].name == attrname );

#ifdef WITH_CHANGELIST
 	    EdgeRef er = gc.getEdge(vid1, vid2, cl);
	    if (!er.valid())
	    {
		dest = gc.getProperties().edgeattr[attrid];
		return false;
	    }
	    else
	    {
		dest = er.getAttr(attrid);
	    }
#else
	    const GraphData::Edge *ep = static_cast<const GraphData&>(gc).getEdge(vid1, vid2);
	    
	    if (!ep)
	    {
		dest = gc.getProperties().edgeattr[attrid];
		return false;
	    }
	    else
	    {
		dest = ep->getAttr(attrid, gc);
	    }
#endif
	}
	return true;
    }

    /// returns false, because value isnt constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (dest) *dest = attrval;
	return false;
    }

    /// the attribute name
    virtual std::string toString() const
    {
	return attrname;
    }
};

/// Parse tree node representing the vertex or edge attribute value
/// place-holder. This node will get values from the two vertices attached to
/// an edge.
class PNAttributeDotName : public ParseNode
{
private:
    /// string name of the vertex attribute
    std::string		fullname, attrname;

    /// vertex attribute id
    unsigned int	attrid;

    /// vertex attribute type and default value
    AnyType		attrval;

    /// false if source vertex, false if target vertex
    bool		src_or_tgt;

public:
    /// constructor from the string received from the parser.
    PNAttributeDotName(std::string _attrname, const GraphProperties &gp, vertex_or_edge_t voe)
	: ParseNode(), fullname(_attrname), attrval(ATTRTYPE_INVALID)
    {
	if (voe != VE_EDGE) {
	    assert(0);
	    throw(AttributeParseException("Program error: AttributeDotName not applicable to a vertex"));
	}
	
	// find the dot
	size_t dotloc = _attrname.find('.');
	assert(dotloc != std::string::npos);

	// check if the first component is either "src" or "tgt" or "s" or "t":
	std::string comp1 = std::string(_attrname.begin(), _attrname.begin()+dotloc);

	if (comp1 == "src" or comp1 == "s") src_or_tgt = false;
	else if (comp1 == "tgt" or comp1 == "t") src_or_tgt = true;
	else {
	    throw(AttributeParseException("Unknown first component in attribute name " + attrname + ". Remember they are case-sensitive."));
	}

	std::string comp2 = std::string(_attrname.begin()+dotloc+1, _attrname.end());

	if (comp2 == "id") {
	    attrname = comp2;
	    attrid = UINT_MAX;
	    attrval = 0U;
	    return;
	}

	int _attrid = gp.vertexattr.lookupAttributeName(comp2);
	if (_attrid < 0)
	    throw(AttributeParseException("Unknown second component in attribute name " + comp2 + ". Remember they are case-sensitive."));

	attrname = comp2;
	attrid = _attrid;
	attrval = gp.vertexattr[attrid];
    }

    /// returns the value of the specified attribute in the current vertex or
    /// edge attribute value chain.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	if (not wantedge) { // vertex attribute selection
	    assert(0);
	}
	else
	{
	    if (attrid == UINT_MAX) {
		dest = src_or_tgt ? vid1 : vid2;
		return true;
	    }

	    // edge attribute selector: get either source or target vertex object

	    assert( attrid < gc.getProperties().vertexattr.size() );
	    assert( gc.getProperties().vertexattr[attrid].name == attrname );

#ifdef WITH_CHANGELIST
	    VertexRef vr = gc.getVertex( src_or_tgt == false ? vid1 : vid2 , cl);
	   
	    if (!vr.valid())
	    {
		dest = gc.getProperties().vertexattr[attrid];
		return false;
	    }
	    else
	    {
		dest = vr.getAttr(attrid);
	    }
#else
	    if (!gc.existVertex( src_or_tgt == false ? vid1 : vid2))
		return false;

	    const GraphData::Vertex* vp = static_cast<const GraphData&>(gc).getVertex( src_or_tgt == false ? vid1 : vid2 );
	    
	    dest = vp->getAttr(attrid, gc);
#endif
	}
	return true;
    }

    /// returns false, because value isnt constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (dest) *dest = attrval;
	return false;
    }

    /// the attribute name
    virtual std::string toString() const
    {
	return fullname;
    }
};

/// Parse tree node representing an unary operator (currently only - for
/// numbers).
class PNUnaryArithmExpr : public ParseNode
{
private:
    /// pointer to the single operand
    const ParseNode 	*opand;

    /// arithmetic operation to perform.
    /// further optimization would be to create an extra class for each op
    char	op;

public:
    PNUnaryArithmExpr(const ParseNode* _opand, char _op)
	: ParseNode(), opand(_opand), op(_op)
    {
	if (op == 'n') op = '!';
    }

    /// recursive parse tree delete
    virtual ~PNUnaryArithmExpr()
    {
	delete opand;
    }

    /// applies the operator to the recursive calculated value
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    { 
	bool b = opand->getValue(dest, gc, cl, wantedge, vid1, vid2);

	if (op == '-') {
	    dest = -dest;	    
	}
	else if (op == '!')
	{
	    // this should not happend, as types are constant in the parse tree
	    if (dest.getType() != ATTRTYPE_BOOL)
		throw(AttributeParseException("Invalid operand for !. Operand must be of type bool."));

	    dest = -dest;
 	}
	else {
	    assert(op == '+');
	}

	return b;
    }

    /// calculates subnodes and returns result if the operator can be applied
    virtual bool getConstVal(AnyType *dest) const
    {
	if (!dest) return false;

	bool b = opand->getConstVal(dest);

	if (op == '-') {
	    *dest = -(*dest);
	}
	else if (op == '!')
	{
	    if (dest->getType() != ATTRTYPE_BOOL)
		throw(AttributeParseException("Invalid operand for !. Operand must be of type bool."));

	    *dest = -(*dest);
	}
	else {
	    assert(op == '+');
	}
	
	return b;
    }

    virtual std::string toString() const
    {
	return std::string("(") + op + " " + opand->toString() + ")";
    }
};

/// Parse tree node representing a binary operators: +, -, * and / for numeric
/// values. This node has two children.
class PNBinaryArithmExpr : public ParseNode
{
private:
    /// pointers to the two child parse trees.
    const ParseNode 	*left, *right;

    /// arithmetic operation to perform: left op right
    /// further optimization would be to create an extra class for each op
    char	op;

public:
    PNBinaryArithmExpr(const ParseNode* _left,
		       const ParseNode* _right,
		       char _op)
	: ParseNode(),
	  left(_left), right(_right), op(_op)
    { }

    /// recursive parse tree delete
    virtual ~PNBinaryArithmExpr()
    {
	delete left;
	delete right;
    }

    /// applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyType's operators.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getValue(vl, gc, cl, wantedge, vid1, vid2);
	bool br = right->getValue(vr, gc, cl , wantedge, vid1, vid2);

	if (op == '+') {
	    dest = vl + vr;
	}
	else if (op == '-') {
	    dest = vl - vr;
	}
	else if (op == '*') {
	    dest = vl * vr;
	}
	else if (op == '/') {
	    dest = vl / vr;
	}

	return (bl and br);
    }

    /// returns false because this node isnt always constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (!dest) return false;

	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getConstVal(&vl);
	bool br = right->getConstVal(&vr);

	if (op == '+') {
	    *dest = vl + vr;
	}
	else if (op == '-') {
	    *dest = vl - vr;
	}
	else if (op == '*') {
	    *dest = vl * vr;
	}
	else if (op == '/') {
	    *dest = vl / vr;
	}

	return (bl and br);
    }

    /// string (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

/// Parse tree node handeling type conversions within the tree.
class PNCastExpr : public ParseNode
{
private:
    /// child tree of which the return value should be casted.
    const ParseNode 	*val;

    /// attribute type to cast the value to.
    attrtype_t	type;

public:
    PNCastExpr(const ParseNode* _val,
	       attrtype_t _type)
	: ParseNode(),
	  val(_val), type(_type)
    { }

    /// recursive parse tree delete
    virtual ~PNCastExpr()
    {
	delete val;
    }

    /// recursive calculation of the value and subsequent casting via AnyType's
    /// convertType method.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	bool b = val->getValue(dest, gc, cl, wantedge, vid1, vid2);
	dest.convertType(type);
	return b;
    }

    /// returns false because this node isnt always constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (!dest) return false;

	bool b = val->getConstVal(dest);
	dest->convertType(type);
	return b;
    }

    /// c-like representation of the cast
    virtual std::string toString() const
    {
	return std::string("(") + val->toString() + " cast " + AnyType::getTypeString(type) + std::string(")");
    }
};

/// Parse tree node representing a binary comparison operator: ==, =, !=, <, >,
/// >=, <=, =>, =<. This node has two children.
class PNBinaryComparisonExpr : public ParseNode
{
private:
    /// pointers to the two child parse trees.
    const ParseNode 	*left, *right;

    /// comparison operation to perform: left op right
    enum { EQUAL, NOTEQUAL, LESS, GREATER, LESSEQUAL, GREATEREQUAL } op;

    /// string saved for toString()
    std::string		opstr;

public:
    PNBinaryComparisonExpr(const ParseNode* _left,
			   const ParseNode* _right,
			   std::string _op)
	: ParseNode(),
	  left(_left), right(_right), opstr(_op)
    {
	if (_op == "==" or _op == "=")
	    op = EQUAL;
	else if (_op == "!=")
	    op = NOTEQUAL;
	else if (_op == "<")
	    op = LESS;
	else if (_op == ">")
	    op = GREATER;
	else if (_op == "<=" or _op == "=<")
	    op = LESSEQUAL;
	else if (_op == ">=" or _op == "=>")
	    op = GREATEREQUAL;
	else
	    throw(AttributeParseException("Program Error: invalid binary comparision operator."));
    }

    /// recursive parse tree delete
    virtual ~PNBinaryComparisonExpr()
    {
	delete left;
	delete right;
    }

    /// applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyType's operators.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getValue(vl, gc, cl, wantedge, vid1, vid2);
	bool br = right->getValue(vr, gc, cl , wantedge, vid1, vid2);

	dest.resetType(ATTRTYPE_BOOL);

	switch(op)
	{
	case EQUAL:
	    dest = AnyType( vl.op_equal_to(vr) );
	    break;

	case NOTEQUAL:
	    dest = AnyType( vl.op_not_equal_to(vr) );
	    break;

	case LESS:
	    dest = AnyType( vl.op_less(vr) );
	    break;

	case GREATER:
	    dest = AnyType( vl.op_greater(vr) );
	    break;

	case LESSEQUAL:
	    dest = AnyType( vl.op_less_equal(vr) );
	    break;

	case GREATEREQUAL:
	    dest = AnyType( vl.op_greater_equal(vr) );
	    break;

	default:
	    assert(0);
	}

	return (bl and br);
    }

    /// returns false because this node isnt always constant
    virtual bool getConstVal(AnyType *dest) const
    {
	if (!dest) return false;

	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getConstVal(&vl);
	bool br = right->getConstVal(&vr);

	switch(op)
	{
	case EQUAL:
	    *dest = AnyType( vl.op_equal_to(vr) );
	    break;

	case NOTEQUAL:
	    *dest = AnyType( vl.op_not_equal_to(vr) );
	    break;

	case LESS:
	    *dest = AnyType( vl.op_less(vr) );
	    break;

	case GREATER:
	    *dest = AnyType( vl.op_greater(vr) );
	    break;

	case LESSEQUAL:
	    *dest = AnyType( vl.op_less_equal(vr) );
	    break;

	case GREATEREQUAL:
	    *dest = AnyType( vl.op_greater_equal(vr) );
	    break;

	default:
	    assert(0);
	}

	return (bl and br);
    }

    /// string (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + opstr + " " + right->toString() + ")";
    }
};

/// Parse tree node representing a binary logic operator: and, or, &&, ||. This
/// node has two children.
class PNBinaryLogicExpr : public ParseNode
{
private:
    /// pointers to the two child parse trees.
    const ParseNode 	*left, *right;

    /// comparison operation to perform: left op right
    enum { OP_AND, OP_OR } op;

public:
    PNBinaryLogicExpr(const ParseNode* _left,
		      const ParseNode* _right,
		      std::string _op)
	: ParseNode(),
	  left(_left), right(_right)
    {
	if (_op == "and" or _op == "&&")
	    op = OP_AND;
	else if (_op == "or" or _op == "||")
	    op = OP_OR;
	else
	    throw(AttributeParseException("Program Error: invalid binary logic operator."));
    }

    /// recursive parse tree delete
    virtual ~PNBinaryLogicExpr()
    {
	if (left) delete left;
	if (right) delete right;
    }

    /// calculate the operator
    inline bool do_operator(bool left, bool right) const
    {
	if (op == OP_AND) return left && right;
	else if (op == OP_OR) return left || right;
	else return false;
    }

    /// applies the operator to the two recursive calculated values. The actual
    /// switching between types is handled by AnyType's operators.
    virtual bool getValue(AnyType &dest, const class GraphContainer &gc, const class Changelist &cl,
			  vertex_or_edge_t wantedge, unsigned int vid1, unsigned int vid2) const
    {
	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getValue(vl, gc, cl, wantedge, vid1, vid2);
	bool br = right->getValue(vr, gc, cl , wantedge, vid1, vid2);

	// these should never happen.
	if (vl.getType() != ATTRTYPE_BOOL)
	    throw(AttributeParseException(std::string("Invalid left operand for ") + (op == OP_AND ? "&&" : "||") + ". Both operands must be of type bool."));
	if (vr.getType() != ATTRTYPE_BOOL)
	    throw(AttributeParseException(std::string("Invalid right operand for ") + (op == OP_AND ? "&&" : "||") + ". Both operands must be of type bool."));

	int bvl = vl.getInteger();
	int bvr = vr.getInteger();

	dest = AnyType( do_operator(bvl, bvr) );

	return (bl and br); // this is not the constness, it is the validitiy of the return value.
    }

    virtual bool getConstVal(AnyType *dest) const
    {
	if (!dest) return false; /// returns false because this node isnt always constant

	AnyType vl(ATTRTYPE_INVALID), vr(ATTRTYPE_INVALID);
	
	bool bl = left->getConstVal(&vl);
	bool br = right->getConstVal(&vr);

	if (vl.getType() != ATTRTYPE_BOOL)
	    throw(AttributeParseException(std::string("Invalid left operand for ") + (op == OP_AND ? "&&" : "||") + ". Both operands must be of type bool."));
	if (vr.getType() != ATTRTYPE_BOOL)
	    throw(AttributeParseException(std::string("Invalid right operand for ") + (op == OP_AND ? "&&" : "||") + ". Both operands must be of type bool."));

	int bvl = vl.getInteger();
	int bvr = vr.getInteger();

	*dest = AnyType( do_operator(bvl, bvr) );

	if (op == OP_AND)
	{
	    // true if either both ops are themselves constant, or if either of
	    // the ops are constant and evaluates to false.
	    return (bl and br) or (bl and !bvl) or (br and !bvr);
	}
	else if (op == OP_OR)
	{
	    // true if either both ops are themselves constant, or if either of
	    // the ops is constant and evaultes to true.
	    return (bl and br) or (bl and bvl) or (br and bvr);
	}
	else {
	    assert(0);
	    return false;
	}
    }

    /// string (operandA op operandB)
    virtual std::string toString() const
    {
	return std::string("(") + left->toString() + " " + (op == OP_AND ? "&&" : "||") + " " + right->toString() + ")";
    }

    /// detach left node
    inline const ParseNode* detach_left()
    {
	const ParseNode *n = left;
	left = NULL;
	return n;
    }

    /// detach right node
    inline const ParseNode* detach_right()
    {
	const ParseNode *n = right;
	right = NULL;
	return n;
    }
};

// *** Functions which translate the resulting parse tree into our expression
// *** tree, simultaneously folding constants.

typedef char const* InputIterT;

typedef tree_match<InputIterT> ParseTreeMatchT;

typedef ParseTreeMatchT::const_tree_iterator TreeIterT;

/// build_expr is the constructor method to create a parse tree from the
/// ast-tree returned by the spirit parser.
static const ParseNode* build_expr(TreeIterT const& i, const GraphProperties &gp, vertex_or_edge_t voe)
{
#ifdef BOOST_SPIRIT_DEBUG
    std::cout << "In build_expr. i->value = " <<
        std::string(i->value.begin(), i->value.end()) <<
        " i->children.size() = " << i->children.size() << 
	" i->value.id = " << i->value.id().to_long() << std::endl;
#endif

    switch(i->value.id().to_long())
    {
    // *** Constant node cases
    case integer_const_id:
    {
	return new PNConstant(ATTRTYPE_INTEGER,
			      std::string(i->value.begin(), i->value.end()));
    }

    case double_const_id:
    {
	return new PNConstant(ATTRTYPE_DOUBLE,
			      std::string(i->value.begin(), i->value.end()));
    }

    case string_const_id:
    {
	return new PNConstant(ATTRTYPE_STRING,
			      std::string(i->value.begin(), i->value.end()));
    }

    // *** Arithmetic node cases

    case add_expr_id:
    case mul_expr_id:
    {
	char arithop = *i->value.begin();
	assert(i->children.size() == 2);

	// we need auto_ptr because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin(), gp, voe) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1, gp, voe) );

	if (left->getConstVal(NULL) and right->getConstVal(NULL))
	{
	    // construct a constant node
	    PNBinaryArithmExpr tmpnode(left.release(), right.release(), arithop);
	    AnyType both(ATTRTYPE_INVALID);

	    tmpnode.getConstVal(&both);

	    // left and right are deleted by tmpnode's deconstructor

	    return new PNConstant(both);
	}
	else
	{
	    // calculation node
	    return new PNBinaryArithmExpr(left.release(), right.release(), arithop);
        }
    }

    case unary_expr_id:
    {
	char arithop = *i->value.begin();
	assert(i->children.size() == 1);

	const ParseNode *val = build_expr(i->children.begin(), gp, voe);

	if (val->getConstVal(NULL))
	{
	    // construct a constant node
	    PNUnaryArithmExpr tmpnode(val, arithop);
	    AnyType constval(ATTRTYPE_INVALID);

	    tmpnode.getConstVal(&constval);

	    return new PNConstant(constval);
	}
	else
	{
	    // calculation node
	    return new PNUnaryArithmExpr(val, arithop);
	}
    }

    // *** Cast node case

    case cast_spec_id:
    {
	assert(i->children.size() == 1);

	std::string tname(i->value.begin(), i->value.end());
	attrtype_t at = AnyType::stringToType(tname);
	
	const ParseNode *val = build_expr(i->children.begin(), gp, voe);

	if (val->getConstVal(NULL))
	{
	    // construct a constant node
	    PNCastExpr tmpnode(val, at);

	    AnyType constval(ATTRTYPE_INVALID);

	    tmpnode.getConstVal(&constval);

	    return new PNConstant(constval);
	}
	else
	{
	    return new PNCastExpr(val, at);
	}
    }

    // *** Binary Comparison Operator

    case comp_expr_id:
    {
	assert(i->children.size() == 2);

	std::string arithop(i->value.begin(), i->value.end());

	// we need auto_ptr because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin(), gp, voe) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1, gp, voe) );

	if (left->getConstVal(NULL) and right->getConstVal(NULL))
	{
	    // construct a constant node
	    PNBinaryComparisonExpr tmpnode(left.release(), right.release(), arithop);
	    AnyType both(ATTRTYPE_INVALID);

	    tmpnode.getConstVal(&both);

	    // left and right are deleted by tmpnode's deconstructor

	    return new PNConstant(both);
	}
	else
	{
	    // calculation node
	    return new PNBinaryComparisonExpr(left.release(), right.release(), arithop);
        }
    }

    // *** Binary Logic Operator

    case and_expr_id:
    case or_expr_id:
    {
	assert(i->children.size() == 2);

	std::string logicop(i->value.begin(), i->value.end());

	// we need auto_ptr because of possible parse exceptions in build_expr.

	std::auto_ptr<const ParseNode> left( build_expr(i->children.begin(), gp, voe) );
	std::auto_ptr<const ParseNode> right( build_expr(i->children.begin()+1, gp, voe) );

	bool constleft = left->getConstVal(NULL);
	bool constright = right->getConstVal(NULL);

	// a logical node is constant if one of the two ops is constant. so we
	// construct a calculation node and check later.
	std::auto_ptr<PNBinaryLogicExpr> node( new PNBinaryLogicExpr(left.release(), right.release(), logicop) );

	if (constleft or constright)
	{
	    AnyType both(ATTRTYPE_INVALID);

	    // test if the node is really const.
	    if (node->getConstVal(&both))
	    {
		// return a constant node instead, node will be deleted by
		// auto_ptr, left,right by node's destructor.
		return new PNConstant(both);
	    }
	}
	if (constleft)
	{
	    // left node is constant, but the evaluation is not
	    // -> only right node is meaningful.
	    return node->detach_right();
	}
	if (constright)
	{
	    // right node is constant, but the evaluation is not
	    // -> only left node is meaningful.
	    return node->detach_left();
	}

	return node.release();
    }

    // *** Vertex/Edge Attribute place-holder

    case attrname_id:
    {
	assert(i->children.size() == 0);

	std::string attrname(i->value.begin(), i->value.end());

	size_t dotloc = attrname.find('.');
	if (dotloc == std::string::npos)
	{
	    // usual attribute string
	    return new PNAttributeName(attrname, voe == VE_VERTEX ? gp.vertexattr : gp.edgeattr, voe);
	}
	else
	{
	    if (voe != VE_EDGE)
		throw(AttributeParseException("Invalid attribute name " + attrname + " for vertex selection."));

	    // attribute string containing a dot.
	    return new PNAttributeDotName(attrname, gp, voe);
	}	
    }

    default:
	throw(AttributeParseException("Unknown ast parse tree node found. This should never happen."));
    }
}

/// build_attrlist construct the vector holding the ParseNode parse
/// tree for each requested attribute.
bool build_attrlist(TreeIterT const &i, const GraphProperties &gp, vertex_or_edge_t voe, class AttributeSelectorList &sellist)
{
#ifdef BOOST_SPIRIT_DEBUG
    std::cout << "In build_attrlist. i->value = " <<
        std::string(i->value.begin(), i->value.end()) <<
        " i->children.size() = " << i->children.size() << 
	" i->value.id = " << i->value.id().to_long() << std::endl;
#endif

    sellist.clear();

    switch(i->value.id().to_long())
    {
    case attrlist_star_id:
    {
	sellist.selectStar(voe == VE_VERTEX ? gp.vertexattr: gp.edgeattr, voe);
	break;
    }

    case attrlist_exprlist_id:
    {
	for(TreeIterT ci = i->children.begin(); ci != i->children.end(); ++ci)
	{
	    const ParseNode *vas = build_expr(ci, gp, voe);
	    //std::cout << "parsed: " << vas->toString() << "\n";

	    sellist.addAttrsel(vas);
	}
	break;
    }

    default:
    {
	// for attribute lists with only one entry, the ast tree does not
	// contain an attrlist_exprlist_id

	const ParseNode *vas = build_expr(i, gp, voe);

	//std::cout << "parsed: " << vas->toString() << "\n";

	sellist.addAttrsel(vas);
	break;
    }
    }

    sellist.calcAttributeLookups();
    return true;
}

} // namespace AttributeParser

// *** AttributeSelectorList methods

void AttributeSelectorList::selectStar(const AttributePropertiesList &otherlist, vertex_or_edge_t voe)
{
    // copy the attributes from otherlist into this and create simple PNAttribute nodes.

    for(AttributePropertiesList::const_iterator ai = otherlist.begin(); ai != otherlist.end(); ai++)
    {
	// put into the parent-class
	attrlist::push_back(*ai);

	// create a selector node
	sellist::push_back( new AttributeParser::PNAttributeName(ai->name, otherlist, voe) );
    }

    assert( attrlist::size() == sellist::size() );
}

void AttributeSelectorList::addAttrsel(const class ParseNode *attrsel)
{
    assert(attrsel);

    // calculate the resulting type and default value.
    AnyType rtype(ATTRTYPE_INVALID);
    attrsel->getConstVal(&rtype);

    std::string rstring = attrsel->toString();
    
    // put into parent class AttributePropertiesList a nearest representation
    // of this selection as an attribute.
    attrlist::push_back( AttributeProperties(rstring, rtype) );

    // and save this selection tree
    sellist::push_back( attrsel );

    assert( attrlist::size() == sellist::size() );
}

#ifdef BOOST_SPIRIT_DEBUG
template <typename TreeInfo>
static inline void debug_dump_xml(const std::string &input, const TreeInfo &info)
{
    using namespace AttributeParser;

    // used by the xml dumper to label the nodes

    std::map<parser_id, std::string> rule_names;

    rule_names[integer_const_id] = "integer_const";
    rule_names[double_const_id] = "double_const";
    rule_names[string_const_id] = "string_const";
    rule_names[constant_id] = "constant";

    rule_names[function_call_id] = "function_call";
    rule_names[function_identifier_id] = "function_identifier";

    rule_names[attrname_id] = "attrname";

    rule_names[unary_expr_id] = "unary_expr";
    rule_names[mul_expr_id] = "mul_expr";
    rule_names[add_expr_id] = "add_expr";

    rule_names[cast_expr_id] = "cast_expr";
    rule_names[cast_spec_id] = "cast_spec";

    rule_names[comp_expr_id] = "comp_expr";
    rule_names[and_expr_id] = "and_expr";
    rule_names[or_expr_id] = "or_expr";

    rule_names[expr_id] = "expr";

    rule_names[attrlist_id] = "attrlist";
    rule_names[attrlist_exprlist_id] = "attrlist_exprlist";
    rule_names[attrlist_star_id] = "attrlist_star";

    rule_names[filter_expr_id] = "filter_expr";

    tree_to_xml(std::cout, info.trees, input.c_str(), rule_names);
}
#endif

bool AttributeSelectorList::parseString(const std::string &input, const class GraphProperties &gp, vertex_or_edge_t voe)
{
    using namespace AttributeParser;

    // instance of the grammar
    AttributeGrammar g;

#ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_GRAMMAR(g);
#endif

    tree_parse_info<> info =
	boost::spirit::ast_parse(input.c_str(),
				 g.use_parser<0>(),	// use first entry point: attrlist
				 boost::spirit::space_p);

    if (not info.full)
    {
	char synstr[256];
	g_snprintf(synstr, sizeof(synstr),
		   "Syntax error at position %d near %.10s",
		   static_cast<int>(info.stop - input.c_str()),
		   info.stop);
	throw (AttributeParseException(synstr));
    }

#ifdef BOOST_SPIRIT_DEBUG
    debug_dump_xml(input, info);
#endif

    return build_attrlist(info.trees.begin(), gp, voe, *this);
}

std::string AttributeSelectorList::toString() const
{
    std::string sl;

    for(sellist::const_iterator i = sellist::begin(); i != sellist::end(); i++) {

	if (i != sellist::begin()) {
	    sl += ", ";
	}

	sl += (*i)->toString();
    }

    return sl;
}

// *** FilterRoot methods

bool FilterRoot::parseString(const std::string &input, const class GraphProperties &gp)
{
    // clear filter
    vertexoredge = VE_VERTEX;
    if (filter) {
	delete filter;
	filter = NULL;
    }

    if (input.size() == 0) return true;

    using namespace AttributeParser;

    // instance of the grammar
    AttributeGrammar g;

#ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_GRAMMAR(g);
#endif

    tree_parse_info<> info =
	boost::spirit::ast_parse(input.c_str(),
				 g.use_parser<1>(),	// use second entry point: filter_expr
				 boost::spirit::space_p);

    if (not info.full)
    {
	char synstr[256];
	g_snprintf(synstr, sizeof(synstr),
		   "Syntax error at position %d near %.10s",
		   static_cast<int>(info.stop - input.c_str()),
		   info.stop);
	throw (AttributeParseException(synstr));
    }

#ifdef BOOST_SPIRIT_DEBUG
    debug_dump_xml(input, info);
#endif

    // used to be build_filterroot:

    // build_filterroot constructs the root object holding either an edge or a
    // vertex filter.
    {
	TreeIterT const &i = info.trees.begin();

#ifdef BOOST_SPIRIT_DEBUG
	std::cout << "In build_filterroot. i->value = " <<
	    std::string(i->value.begin(), i->value.end()) <<
	    " i->children.size() = " << i->children.size() << 
	    " i->value.id = " << i->value.id().to_long() << std::endl;
#endif

	if (i->value.id().to_long() != filter_expr_id)
	    throw(AttributeParseException("Unknown ast parse tree node found. This should never happen."));

	if (i->children.size() == 0)
	{
	    // this is a NULL filter: always true
	    return true;
	}
	else if (i->children.size() == 1)
	{
	    // we have a vertex or an edge filter?
	    char voeletter = *i->value.begin();
	    vertexoredge = (voeletter == 'v') ? VE_VERTEX : VE_EDGE;

	    // build up tree
	    filter = build_expr(i->children.begin(), gp, vertexoredge);

	    // type-check the the filter once, this is needed if the top node
	    // are logic operators
	    AnyType tc(ATTRTYPE_INVALID);
	    filter->getConstVal(&tc);

	    if (tc.getType() != ATTRTYPE_BOOL)
		throw(AttributeParseException("The filter expression must be of type bool. Remember there is no automatic comparison != 0 as in C."));

	    return true;
	}
	else {
	    throw(AttributeParseException("Unknown ast parse tree node found. This should never happen."));
	}
    }
}

std::string FilterRoot::toString() const
{
    if (!filter) return "null-filter";

    return std::string(vertexoredge == VE_VERTEX ? "vertex: " : "edge: ") + filter->toString();
}

} // namespace VGServer
