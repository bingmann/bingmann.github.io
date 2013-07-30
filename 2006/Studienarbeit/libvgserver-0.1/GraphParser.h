// $Id: GraphParser.h 241 2006-07-05 07:29:52Z bingmann $

#ifndef VGS_GraphParser_H
#define VGS_GraphParser_H

#include "ByteInBuffer.h"
#include "AttributeProperties.h"

#include <iostream>

namespace VGServer {

/** Thrown by GraphParser when the data stream is invalid. */
class GraphParserException : std::runtime_error
{
public:
    explicit GraphParserException(const std::string &arg) throw()
	: std::runtime_error(arg)
    {
    }
};

/** A base callback class containing stubs to all the event functions which are
 * called by the GraphParser. Callback classes can derive from this class so
 * they dont have to implement all callback functions. */

class GraphParserCallbackAdapter
{
public:
    typedef std::vector<AnyType> AnyTypeVector;

    void eventStartParse()
    {
    }
    void eventError(const std::string& /* error string */)
    {
    }

    void eventVertexAttributes(const class AttributePropertiesList &)
    {
    }

    void eventEdgeAttributes(const class AttributePropertiesList &)
    {
    }

    void eventStartVertexBody(unsigned int /* vertexnum */)
    {
    }
    void eventVertex(const AnyTypeVector& /* values */)
    {
    }

    void eventStartEdgeBody(unsigned int /* edgenum */)
    {
    }
    void eventEdge(const AnyTypeVector& /* values */)
    {
    }

    /// eventAnimationData is a special function: if it returns true, the
    /// parser will parse the animation data block instantly and call
    /// appropriate functions below. if it returns false, then the
    /// eventAnimationDataBuffer will be called.
    bool eventAnimationParseData()
    {
	return true;
    }

    /// if eventAnimationData() == false. then the parser will call this
    /// function and put the animation data block into the returned buffer for
    /// later parsing.
    ByteBuffer* eventAnimationDataBuffer()
    {
	return NULL;
    }

    void eventStartAnimation(unsigned int /* frames */)
    {
    }
    void eventStartAnimationFrame(unsigned int /* framenum */,
				  unsigned int /* operationsnum */)
    {
    }
	
    void eventAnimationAddVertex(unsigned int /* framenum */,
				 const AnyTypeVector& /* values */)
    {
    }
    void eventAnimationSetVertex(unsigned int /* framenum */,
				 const AnyTypeVector& /* old values */,
				 const AnyTypeVector& /* new values */)
    {
    }
    void eventAnimationDelVertex(unsigned int /* framenum */,
				 const AnyTypeVector& /* values */)
    {
    }
	
    void eventAnimationAddEdge(unsigned int /* framenum */,
			       const AnyTypeVector& /* values */)
    {
    }
    void eventAnimationSetEdge(unsigned int /* framenum */,
			       const AnyTypeVector& /* old values */,
			       const AnyTypeVector& /* new values */)
    {
    }
    void eventAnimationDelEdge(unsigned int /* framenum */,
			       const AnyTypeVector& /* values */)
    {
    }
};

/** GraphParser is a static polymorphic class used to parse the data streams
 * returned by the server. */

template <class Callback>
class GraphParser
{
protected:
    /// link to the callback used during processing
    Callback	&callback;

    // save the two attribute properties list for later deserialization of the
    // body data.
    AttributePropertiesList vertexattr, edgeattr;

public:
    /// initialize the parser with this callback object
    GraphParser(Callback &cb)
	: callback(cb)
    {
    }

    /// initialize the parser for animation parsing with this callback object
    /// and the two attribute lists
    GraphParser(Callback &cb, const AttributePropertiesList &_vertexattr, const AttributePropertiesList &_edgeattr)
	: callback(cb), vertexattr(_vertexattr), edgeattr(_edgeattr)
    {
    }

    /// parse the given byte input buffer and call the given callback class
    /// for each event. this creates a temporary GraphParser object.
    static bool parse(Callback &cb, const ByteBuffer &bb)
    {
	GraphParser gp (cb);

	return gp.parse(bb);
    }

    /// parse the given byte input buffer and call the connected callback class
    /// for each event
    bool parse(const ByteBuffer &bbdata)
    {
	ByteInBuffer bb (bbdata);

	callback.eventStartParse();

        // see if the version signature is correct
        unsigned int versig = (bb.remaining() > 4) ? bb.fetch_unsigned_int() : 0;
        if (versig != 0x00010000 )
            throw GraphParserException("Invalid binary format version encountered");

	// get the first char describing the section
        while (bb.remaining() > 0)
        {
            unsigned char sect = bb.fetch_unsigned_char();
            
            switch(sect)
            {
            case 0: // section=0 is the termination
                return true;
            
            case 1: // section 1 are the vertex attribute properties 
                parseVertexAttrlist(bb);
                break;
                	
            case 2: // section 2 are the edge attribute properties
                parseEdgeAttrlist(bb);
                break;
                
            case 3: // section 3 is the vertex body data
                parseVertexBodyData(bb);
                break;
                
            case 4: // section 4 is the edge body data
                parseEdgeBodyData(bb);
                break;
                
            case 5: // section 5 is the animation sequence
            	parseAnimationStart(bb);
            	break;
            	
            case 0x10: // section 0x10 is the directed flag within GraphProperties
            	parseGraphProperties(bb);
            	break;
                
            case 0xFF: // error section contains a single string
	    {
		std::string errstr = bb.fetchString();
            	callback.eventError(errstr);
                break;
	    }
                    
            default:
                throw GraphParserException("Invalid section within binary format");
            }
        }

        if (bb.remaining() > 0)
        {
            std::cerr << "Incomplete deserialization\n";
            callback.eventError("Incomplete deserialization");
            return false;
        }
        return true;
    }

    /// parse the given animation buffer which was recorded by
    /// eventAnimationDataBuffer(). this creates a temporary GraphParser
    /// object.
    static void parseAnimation(Callback &cb, const ByteBuffer &bb,
			       const AttributePropertiesList &vertexattr, const AttributePropertiesList &edgeattr)
    {
	GraphParser gp (cb, vertexattr, edgeattr);

	return gp.parseAnimation(bb);
    }

    /// parse the given animation buffer which was recorded by
    /// eventAnimationDataBuffer(). be aware that the parser requires
    /// initialization of vertexattr and edgeattr
    void parseAnimation(const ByteBuffer &bbdata)
    {
	ByteInBuffer bb (bbdata);

	assert(vertexattr.defbytes > 0);
	assert(edgeattr.defbytes > 0);

	parseAnimationData(bb);
    }

protected:
    // *** Internal functions parsing various parts of the data stream

    void parseVertexAttrlist(class ByteInBuffer &bb)
    {
	parseAttrlist(bb, vertexattr);
        callback.eventVertexAttributes(static_cast<const AttributePropertiesList&>(vertexattr));
    }

    void parseEdgeAttrlist(class ByteInBuffer &bb)
    {
        parseAttrlist(bb, edgeattr);
        callback.eventEdgeAttributes(static_cast<const AttributePropertiesList&>(edgeattr));
    }

    void parseAttrlist(class ByteInBuffer &bb, class AttributePropertiesList &apl)
    {
	apl.clear();

	unsigned int attrlen = bb.fetch_unsigned_short();

	apl.defbytes = bb.fetch_unsigned_char();

        for(unsigned int ai = 0; ai < attrlen; ai++)
        {
	    attrtype_t at = static_cast<attrtype_t>(bb.fetch_unsigned_char());

            if (!AnyType::isValidAttrtype(at))
                throw GraphParserException("Invalid attribute type id encountered");

	    std::string name = bb.fetchString();
            
            apl.push_back( AttributeProperties(name, at) );
            
            // read default value
            if (at == ATTRTYPE_BOOL) {
                apl.back().setInteger(bb.fetch_unsigned_char());
            }
            else {
                bb.fetchAnyType(apl.back());
            }
        }
    }
    
    static bool getBit(int bitnum, char *bitfield)
    {
        int idx = bitnum / 8;
        bitnum %= 8;
        return (bitfield[idx] & (1 << bitnum)) != 0;        
    }

    void parseVertexBodyData(class ByteInBuffer &bb)
    {
        unsigned int objnum = bb.fetch_unsigned_int();
        callback.eventStartVertexBody(objnum);
        
	char defbits[vertexattr.defbytes];
	std::vector<AnyType> values;
	values.resize(vertexattr.size(), AnyType(ATTRTYPE_INVALID));

        for(unsigned int oi = 0; oi < objnum; oi++)
        {
            // read the default bitfield
            bb.fetchBytes(defbits, vertexattr.defbytes);

            for(unsigned int ai = 0; ai < vertexattr.size(); ai++)
            {
                if (vertexattr[ai].getType() == ATTRTYPE_BOOL) {
                    values[ai].resetType(ATTRTYPE_BOOL);
		    values[ai].setInteger( getBit(ai, defbits) );
                }
                else if (getBit(ai, defbits)) {
                    // default bit is set. so put the default value into the object array
                    values[ai] = vertexattr[ai];
                }
                else {
                    values[ai].resetType(vertexattr[ai].getType());
		    bb.fetchAnyType(values[ai]);
                }
            }
            
            callback.eventVertex(values);
        }
    }

    void parseEdgeBodyData(class ByteInBuffer &bb)
    {
        unsigned int objnum = bb.fetch_unsigned_int();
        callback.eventStartEdgeBody(objnum);
        
	char defbits[edgeattr.defbytes];
	std::vector<AnyType> values;
	values.resize(edgeattr.size(), AnyType(ATTRTYPE_INVALID));

        for(unsigned int oi = 0; oi < objnum; oi++)
        {
            // read the default bitfield
            bb.fetchBytes(defbits, edgeattr.defbytes);

            for(unsigned int ai = 0; ai < edgeattr.size(); ai++)
            {
                if (edgeattr[ai].getType() == ATTRTYPE_BOOL) {
                    values[ai].resetType(ATTRTYPE_BOOL);
		    values[ai].setInteger( getBit(ai, defbits) );
                }
                else if (getBit(ai, defbits)) {
                    // default bit is set. so put the default value into the object array
                    values[ai] = edgeattr[ai];
                }
                else {
                    values[ai].resetType(edgeattr[ai].getType());
		    bb.fetchAnyType(values[ai]);
                }
            }
            
            callback.eventEdge(values);
        }
    }
    
    void parseAnimationStart(class ByteInBuffer &bb)
    {
	unsigned int anisize = bb.fetch_unsigned_int();

	if (callback.eventAnimationParseData())
	{
	    parseAnimationData(bb);
	}
	else
	{
	    // save the animation data block for later parsing

	    ByteBuffer *anidata = callback.eventAnimationDataBuffer();
	    assert(anidata);
	    if (!anidata) return;

	    bb.fetchBytes(*anidata, anisize);
	}
    }

    void parseAnimationData(class  ByteInBuffer &bb)
    {
	unsigned int framenum = bb.fetch_unsigned_int();
	callback.eventStartAnimation(framenum);

        for(unsigned int fn = 0; fn < framenum; fn++)
        {
	    parseAnimationFrame(bb, fn);
	}	
    }

    void parseAnimationFrame(class ByteInBuffer &bb, unsigned int framenum)
    {
	unsigned int opnum = bb.fetch_unsigned_int();
	callback.eventStartAnimationFrame(framenum, opnum);
	
	std::vector<AnyType> oldvals, newvals;

	for(unsigned int on = 0; on < opnum; on++)
	{
            unsigned char opid = bb.fetch_unsigned_char();
            
            switch(opid)
            {
	    default:
            case 0: // invalid id
		throw GraphParserException("Invalid animation operation id encountered.");
	    
	    case 1: // CHG_ADDVERTEX
		parseAnimationVertexData(bb, newvals);
		callback.eventAnimationAddVertex(framenum, newvals);
		break;
    			
	    case 2: // CHG_SETVERTEX
		parseAnimationVertexData(bb, oldvals);
		parseAnimationVertexData(bb, newvals);
		callback.eventAnimationSetVertex(framenum, oldvals, newvals);
		break;

	    case 3: // CHG_DELVERTEX
		parseAnimationVertexData(bb, oldvals);
		callback.eventAnimationDelVertex(framenum, oldvals);
		break;
    			
	    case 4: // CHG_ADDEDGE
		parseAnimationEdgeData(bb, newvals);
		callback.eventAnimationAddEdge(framenum, newvals);
		break;
    			
	    case 5: // CHG_SETEDGE
		parseAnimationEdgeData(bb, oldvals);
		parseAnimationEdgeData(bb, newvals);
		callback.eventAnimationSetEdge(framenum, oldvals, newvals);
		break;
    			
	    case 6: // CHG_DELEDGE
		parseAnimationEdgeData(bb, oldvals);
		callback.eventAnimationDelEdge(framenum, oldvals);
		break;
	    }
	}
    }

    void parseAnimationVertexData(class ByteInBuffer &bb, std::vector<AnyType>& values)
    {
	char defbits[vertexattr.defbytes];
	values.resize(vertexattr.size(), AnyType(ATTRTYPE_INVALID));

	// read the default bitfield
	bb.fetchBytes(defbits, vertexattr.defbytes);

	for(unsigned int ai = 0; ai < vertexattr.size(); ai++)
	{
	    if (vertexattr[ai].getType() == ATTRTYPE_BOOL) {
		values[ai].resetType(ATTRTYPE_BOOL);
		values[ai].setInteger( getBit(ai, defbits) );
	    }
	    else if (getBit(ai, defbits)) {
		// default bit is set. so put the default value into the object array
		values[ai] = vertexattr[ai];
	    }
	    else {
		values[ai].resetType(vertexattr[ai].getType());
		bb.fetchAnyType(values[ai]);
	    }
	}
    }

    void parseAnimationEdgeData(class ByteInBuffer &bb, std::vector<AnyType>& values)
    {
	char defbits[edgeattr.defbytes];
	values.resize(edgeattr.size(), AnyType(ATTRTYPE_INVALID));

	// read the default bitfield
	bb.fetchBytes(defbits, edgeattr.defbytes);

	for(unsigned int ai = 0; ai < edgeattr.size(); ai++)
	{
	    if (edgeattr[ai].getType() == ATTRTYPE_BOOL) {
		values[ai].resetType(ATTRTYPE_BOOL);
		values[ai].setInteger( getBit(ai, defbits) );
	    }
	    else if (getBit(ai, defbits)) {
		// default bit is set. so put the default value into the object array
		values[ai] = edgeattr[ai];
	    }
	    else {
		values[ai].resetType(edgeattr[ai].getType());
		bb.fetchAnyType(values[ai]);
	    }
	}
    }
    
    void parseGraphProperties(class ByteInBuffer &bb)
    {
	/*unsigned char directed = */ bb.fetch_unsigned_char();
    }
};

} // namespace VGServer

#endif // VGS_GraphParser_H
