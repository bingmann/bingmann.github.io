// $Id: RTree.h 264 2006-08-02 08:14:39Z bingmann $

#ifndef VGS_RTree_H
#define VGS_RTree_H

#include <assert.h>

#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <limits>
#include <fstream>
#include <iostream>
#include <ostream>

#include "GraphTypes.h"

namespace VGServer {

//#define R_TREE_LINEAR
//#define R_TREE_QUADRATIC
#define R_STAR_TREE

/** the outer RTree class implementes a "template namespace" parameterized by
 * the CoordType used in all the nested classes */

//template <typename _CoordType, typename _AreaType>
class RTree
{
public:
    // public typedefs of the template parameters
    //typedef _CoordType	CoordType;
    //typedef _AreaType	AreaType;
    typedef coord_t	CoordType;
    typedef long long	AreaType;

    // *** Structures used in the the R-Tree

    // forward declarations
    class Point;
    class Rect;

    /// a two dimensional point
    class Point
    {
    public:
	CoordType	x, y;

	/// noop constructor
	inline Point()
	{ }

	/// initializing constructor
	inline Point(CoordType _x, CoordType _y)
	    : x(_x), y(_y)
	{ }

	/// square the argument, while applying a type conversion to AreaType
	static inline AreaType square(AreaType a)
	{ return a * a; }

	/// calculate the distance to another point. dont use sqrt().
	inline AreaType getDistanceSquare(const Point &p) const
	{
	    return square( p.x - x ) 
		 + square( p.y - y );
	}

	/// calculate the distance to another point. dont use sqrt().
	inline AreaType getDistanceSquare(CoordType px, CoordType py) const
	{
	    return square( px - x ) 
		 + square( py - y );
	}

	/// calculate the minimum distance to the rectangle. dont use sqrt().
	inline AreaType getMinimumDistanceSquare(const class Rect &r) const
	{
	    AreaType a = 0;

	    if (x < r.x1 || r.x2 < x)
		a += square( std::min(x - r.x1, x - r.x2) );

	    if (y < r.y1 || r.y2 < y)
		a += square( std::min(y - r.y1, y - r.y2) );

	    return a;
	}

	/// calculate the minimum distance to this line. dont use sqrt()
	inline AreaType getDistanceLineSquare(CoordType x1, CoordType y1, CoordType x2, CoordType y2) const
	{
	    // Based on comp.graphics.algorithms' FAQ

	    double l = square(x1 - x2) + square(y1 - y2); // square length of line (x1,y1)-(x2,y2)

	    // scalar product of this point on the line.
	    double r = static_cast<double>((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / l;

	    if (r <= 0)
	    {
		// projection point is beyond the line, so we can use the simple distance
		return square(x1 - x) + square(y1 - y);
	    }
	    else if (r >= 1)
	    {
		// projection point is beyond the line, so we can use the simple distance
		return square(x2 - x) + square(y2 - y);
	    }
	    else
	    {
		// dunno how this works
		double s = static_cast<double>(square( (y1 - y) * (x2 - x1) - (x1 - x) * (y2 - y1) )) / l;
		
		return static_cast<AreaType>(s);
	    }
	}
    };

    /// a two dimensional rectangle class with sufficient caluclation methods
    class Rect
    {
    public:
	CoordType	x1, y1, x2, y2;

	/// noop constructor
	inline Rect()
	{ }

	/// initializing constructor
	inline Rect(CoordType _x1, CoordType _y1, CoordType _x2, CoordType _y2)
	    : x1(_x1), y1(_y1), x2(_x2), y2(_y2)
	{
	    assert(x1 <= x2);
	    assert(y1 <= y2);
	}

	/// check that it is a valid rectangle
	inline bool valid() const
	{
	    return (x1 <= x2) && (y1 <= y2);
	}

	/// equality comparison operator
	inline bool operator==(const Rect &o) const
	{
	    return (x1 == o.x1) && (y1 == o.y1) && (x2 == o.x2) && (y2 == o.y2);
	}

	/// sets the coordinates to the minimum and maximum of the data range,
	/// thus creating an invalid rect on which to start combining others.
	inline void setInfinite()
	{
	    x1 = y1 = std::numeric_limits<CoordType>::max();
	    x2 = y2 = std::numeric_limits<CoordType>::min();
	}

	/// enlarge this rectangle to encompass Rect r.
	inline void combineRect(const Rect &r)
	{
	    x1 = std::min(x1, r.x1);
	    y1 = std::min(y1, r.y1);
	    x2 = std::max(x2, r.x2);
	    y2 = std::max(y2, r.y2);
	}

	/// set this rectangle to the minimum rectangle enclosing both a and b.
	inline void combineRects(const Rect &a, const Rect &b)
	{
	    x1 = std::min(a.x1, b.x1);
	    y1 = std::min(a.y1, b.y1);
	    x2 = std::max(a.x2, b.x2);
	    y2 = std::max(a.y2, b.y2);
	}

	/// intersect this rectangle with a.
	inline void intersectRect(const Rect &a)
	{
	    x1 = std::max(a.x1, x1);
	    y1 = std::max(a.y1, y1);
	    x2 = std::min(a.x2, x2);
	    y2 = std::min(a.y2, y2);
	}

	/// set this rectangle to the minimum rectangle contained by both a and b.
	inline void intersectRects(const Rect &a, const Rect &b)
	{
	    x1 = std::max(a.x1, b.x1);
	    y1 = std::max(a.y1, b.y1);
	    x2 = std::min(a.x2, b.x2);
	    y2 = std::min(a.y2, b.y2);
	}

	/// calculate the enclosed area
	inline AreaType getArea() const
	{
	    return static_cast<AreaType>(x2 - x1) * static_cast<AreaType>(y2 - y1);
	}

	/// calculate the intersecting area of this rectangle with a.
	inline AreaType getIntersectingArea(const Rect &a) const
	{
	    if (x2 < a.x1 || a.x2 < x1 || y2 < a.y2 || a.y2 < y2) return 0;

	    return static_cast<AreaType>(std::min(a.x2, x2) - std::max(a.x1, x1))
		 * static_cast<AreaType>(std::min(a.y2, y2) - std::max(a.y1, y1));
	}

	/// calculate the margin of the rectangle, i.e. the 2*sum of both sides
	inline AreaType getMargin() const
	{
	    return 2 * static_cast<AreaType>( (x2 - x1) + (y2 - y1) );
	}

	/// returns the center point of the rectangle
	inline Point getCenter() const
	{
	    return Point(x1 + (x2 - x1) / 2, y1 + (y2 - y1) / 2);
	}

	/// checks if the other rectangle is contained within this one
	inline bool containsRect(const Rect &r) const
	{
	    return (x1 <= r.x1) && (y1 <= r.y1) && (r.x2 <= x2) && (r.y2 <= y2);
	}
	
	/// checks if the other rectangle touches this one. means if it shares a border.
	inline bool touchesRect(const Rect &r) const
	{
	    // note that this will not work for floating point numbers.
	    return (x1 == r.x1) || (y1 == r.y1) || (r.x2 == x2) || (r.y2 == y2);
	}

	/// checks if the other rectangle intersects this one. means that their
	/// transection is not empty.
	inline bool intersectsRect(const Rect &r) const
	{
	    return (x1 <= r.x2 && x2 >= r.x1 && y1 <= r.y2 && y2 >= r.y1);
	}
    };

    /** Data class used to collect information from one level the tree */
    struct LevelStats
    {
	/// number of nodes in level
	unsigned int nodes;

	/// number of children in node level
        unsigned int children;

	/// number of unused children places
	unsigned int unusedchildren;

	/// bytes used by nodes
	unsigned int bytes;

	/// unused bytes in nodes
	unsigned int unusedbytes;

	/// overlapping area of rectangles within one node
        AreaType overlap;
	
	/// area outside the union of all rectangles within a node
	AreaType wastearea;

	/// zero initialized
	inline LevelStats()
	{
	    nodes = 0;
	    children = 0;
	    unusedchildren = 0;
	    bytes = 0;
	    unusedbytes = 0;
	    overlap = 0;
	    wastearea = 0;
	}

        /// add things up
	inline LevelStats & operator+=(const LevelStats &s)
	{
	    nodes += s.nodes;
	    children += s.children;
	    unusedchildren += s.unusedchildren;
	    bytes += s.bytes;
	    unusedbytes += s.unusedbytes;
	    overlap += s.overlap;
	    wastearea += s.wastearea;
	    return *this;
	}
    };

    /** Data class used to collect statistical information from the tree */
    struct Stats
    {
        /// vector of level stats
	std::vector<LevelStats> level;

	/// zero initialized
	inline Stats()
	{
	}
    };
    
    /** Tree is a class implementing an R*-Tree used to accelerate access to
     * the edges during a getArea query. */
    
    template <typename _DataType, typename _DataTypeCallback>
    class Tree
    {
    public:
	// *** Parameters of the R-Tree

	typedef _DataType		DataType;
	typedef const _DataType&	DataRef;

	typedef _DataTypeCallback	DataTypeCallback;

	typedef unsigned int pageid_t;

	static const unsigned int	pagesize = 32;

	static const unsigned int	pageminfill = 12;

	unsigned int			reinsertnum;

    public:
    
	/// data structed used to order children to reinsert
	struct ReinsertDist
	{
	    unsigned int 	childid;
	    AreaType	distance;

	    static inline bool order_distance(const ReinsertDist &a, const ReinsertDist &b)
	    {
		return a.distance < b.distance;
	    }
	    static inline bool order_childid(const ReinsertDist &a, const ReinsertDist &b)
	    {
		return a.childid < b.childid;
	    }
	};

    protected:
	/// forward declarations
	class LeafNode;
	class InnerNode;

	class InnerNodeData;
	class LeafNodeData;

	/// common data structures found at the beginning of each node page.
	class NodeHead
	{
	public:
	    /// level of this node in the R-Tree. if level != 0, then this node is
	    /// an inner node, and thus has the InnerNode structure
	    unsigned short	level;
	
	    /// number of children places occupied.
	    unsigned short	childnum;

	    /// min bounding rectangle of this node
	    Rect		nodeMBR;

	    // *** Operations within NodeHead

	    /// initialize this node as the empty root node.
	    inline void	initializeRoot()
	    {
		nodeMBR.setInfinite();
		level = 0;
		childnum = 0;
	    }

	    /// initialize this node as a sibling of the node which will be split.
	    inline void	initializeSibling(unsigned int _level)
	    {
		nodeMBR.setInfinite();
		level = _level;
		childnum = 0;
	    }
	};

	/// node page augmented by a data type: either the childMBR + childPageId
	/// for InnerNodes or the childData for LeafNodes. It contains generic
	/// functions used to insert, split nodes containing _NodeData elements.
	template <typename _NodeData>
	class Node : public NodeHead
	{
	public:
	    /// public typedef of the NodeData
	    typedef _NodeData	NodeData;

	    /// typedef of our own type
	    typedef Node<NodeData>	NodeType;

	    /// array of data elements within this node
	    NodeData	children[pagesize];

	    // *** Primitive Operations

	    /// recalculate the MBR of all children. used at various places
	    inline void	recalcChildrenMBR(const Tree &tree, class Rect &dest) const
	    {
		dest.setInfinite();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		    dest.combineRect(children[ci].getMBR(tree));
	    }

	    /// find the child which's MBR needs least enlargement to include
	    /// newrect.
	    unsigned int	findLeastEnlargement(const Tree &tree, const Rect &newrect) const
	    {
		AreaType area = std::numeric_limits<AreaType>::max();
		unsigned int bestchild = std::numeric_limits<unsigned int>::max();

		Rect rt;

		// search through the children rectangles
		for(unsigned int i = 0; i < this->childnum; i++)
		{
		    rt.combineRects(newrect, children[i].getMBR(tree));

		    AreaType enlarge = rt.getArea() - children[i].getMBR(tree).getArea();

		    if (enlarge < area)
		    {
			area = enlarge;
			bestchild = i;
		    }
		    // break the tie between equal enlargements
		    else if (enlarge == area)
		    {
			if (rt.getArea() < children[bestchild].getMBR(tree).getArea())
			{
			    bestchild = i;
			}
		    }
		}
    
		assert( bestchild < this->childnum );

		return bestchild;
	    }

	    /// find the child which's MBR need least overlap enlargement when the
	    /// new rect is added.
	    unsigned int	findLeastOverlap(const Tree &tree, const Rect &newrect) const
	    {
		unsigned int min_bestchild = std::numeric_limits<unsigned int>::max();
		AreaType min_overlap = std::numeric_limits<AreaType>::max();
		AreaType min_enlarge = std::numeric_limits<AreaType>::max();

		Rect rt, ru;

		for (unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    // calculate the needed rectangle
		    rt.combineRects(newrect, children[ci].getMBR(tree));

		    AreaType overlap = 0;

		    // calculate the overlap of the new combine rectangle
		    for(unsigned int cj = 0; cj < this->childnum; cj++)
		    {
			if (ci == cj) continue;

			AreaType isa = rt.getIntersectingArea(children[cj].getMBR(tree));

			if (isa > 0)
			{
			    overlap += isa - children[ci].getMBR(tree).getIntersectingArea(children[cj].getMBR(tree));
			}
		    }

		    // and the area enlargement
		    AreaType enlarge = rt.getArea() - children[ci].getMBR(tree).getArea();

		    // find the minimum overlap
		    if (min_overlap > overlap)
		    {
			min_bestchild = ci;
			min_overlap = overlap;
			min_enlarge = enlarge;
		    }
		    // break ties by choosing the lower area enlargement
		    else if (min_overlap == overlap)
		    {
			if (min_enlarge > enlarge)
			{
			    min_bestchild = ci;
			    min_enlarge = enlarge;
			}
			// break further ties by choosing the rectangle with smaller area
			else if (min_enlarge == enlarge)
			{
			    if (children[min_bestchild].getMBR(tree).getArea() > children[ci].getMBR(tree).getArea())
			    {
				min_bestchild = ci;
				min_enlarge = enlarge;
			    }
			}
		    }
		}

		assert( min_bestchild < this->childnum );

		return min_bestchild;
	    }

	    // *** Simple walking of the tree

	    /// select the leaf in which to insert a new rectangle, and save it's
	    /// path down the tree.
	    class NodeHead*	chooseSubtree(Tree &tree, const Rect &newrect, unsigned int maxlevel, std::stack<pageid_t>& path)
	    {
		if (this->level == maxlevel) return this;

#if defined(R_TREE_LINEAR) || defined(R_TREE_QUADRATIC)

		unsigned int child = findLeastEnlargement(tree, newrect);

#elif defined(R_STAR_TREE)

		unsigned int child;
		if (this->level == 1) // if children nodes are leaves
		{
		    child = findLeastOverlap(tree, newrect);
		}
		else
		{
		    child = findLeastEnlargement(tree, newrect);
		}

#endif

		assert(this->level != 0);
		class InnerNode *in = reinterpret_cast<class InnerNode*>(this);

		pageid_t childpage = in->children[child].page;

		class NodeHead *childnode = tree.pageman.getPage(childpage);
		path.push(childpage);

		if (childnode->level == 0) return childnode;

		return static_cast<class InnerNode*>(childnode)->chooseSubtree(tree, newrect, maxlevel, path);
	    }

	    /// algorithm findLeaf used in deleteRect
	    class LeafNode* findLeaf(Tree &tree, const class LeafNodeData &rmdata, std::queue<pageid_t>& path)
	    {
		if (this->level == 0)
		{
		    LeafNode *ln = reinterpret_cast<LeafNode*>(this);

		    // look through children rectangles and compare data
		    for(unsigned int ci = 0; ci < ln->childnum; ci++)
		    {
			if (ln->children[ci] == rmdata)
			{
			    return ln;
			}
		    }
		}
		else
		{
		    InnerNode *in = reinterpret_cast<InnerNode*>(this);

		    Rect rmrect = rmdata.getMBR(tree);

		    // look through children sub-rectangles and descend into those
		    // enclosing rect.
		    for(unsigned int ci = 0; ci < in->childnum; ci++)
		    {
			if (in->children[ci].getMBR(tree).containsRect(rmrect))
			{
			    NodeHead *cn = tree.pageman.getPage(in->children[ci].page);

			    LeafNode *ret;
			    if (cn->level == 0) 
				ret = static_cast<LeafNode*>(cn)->findLeaf(tree, rmdata, path);
			    else
				ret = static_cast<InnerNode*>(cn)->findLeaf(tree, rmdata, path);

			    if (ret)
			    {
				path.push(in->children[ci].page);
				return ret;
			    }
			}
		    }
		}
		return NULL;
	    }

	    /// write the rectangles out in the fig format. stop at a given level (0 are the leaves)
	    void writeFig(const Tree &tree, std::ostream &o, int maxlevel) const
	    {
		if (static_cast<int>(this->level) < maxlevel) return;

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    if (this->level > 0)
		    {
			// descend into child rectangle
			const InnerNode *in = reinterpret_cast<const InnerNode*>(this);

			NodeHead *cn = tree.pageman.getPage(in->children[ci].page);
		    
			if (cn->level == 0)
			    static_cast<LeafNode*>(cn)->writeFig(tree, o, maxlevel);
			else
			    static_cast<InnerNode*>(cn)->writeFig(tree, o, maxlevel);
		    }

		    const Rect &rect = children[ci].getMBR(tree);

		    if (rect.getArea() == 0) continue; // xfig doesnt like non-rectangles

		    // output rectangle as fig polyline
		    o << "2 2 0 1 " << 32+this->level << " 7 " << 100-this->level << " -1 -1 0.000 0 0 -1 0 0 5\n"
		      << "       "
		      << rect.x1 << " " << rect.y1 << " "
		      << rect.x2 << " " << rect.y1 << " "
		      << rect.x2 << " " << rect.y2 << " "
		      << rect.x1 << " " << rect.y2 << " "
		      << rect.x1 << " " << rect.y1 << "\n";
		}
	    }

	    /// test various invariants of this node and it's children
	    bool		checkNode(const Tree &tree, unsigned int thislevel, unsigned int *numentries) const
	    {
		assert(this->level == thislevel);

		if (this->level > 0)
		{
		    // descend into child rectangles
		    const InnerNode *in = reinterpret_cast<const InnerNode*>(this);

		    for(unsigned int ci = 0; ci < in->childnum; ci++)
		    {
			NodeHead *cn = tree.pageman.getPage(in->children[ci].page);

			if (cn->level == 0) {
			    if (!static_cast<const LeafNode*>(cn)->checkNode(tree, thislevel-1, numentries)) return false;
			}
			else {
			    if (!static_cast<const InnerNode*>(cn)->checkNode(tree, thislevel-1, numentries)) return false;
			}
		    }
		}

		// check that the nodeMBR corresponds to the children MBRs
		Rect r;
		recalcChildrenMBR(tree, r);
		assert(r == this->nodeMBR);

		// check minimum fill
		assert(this->childnum >= pageminfill or this->level == tree.levels);

		// calculate valid entries
		if (this->level == 0)
		    numentries += this->childnum;

		return true;
	    }

	    /// calculate the sum of all overlaps of this node and all children
	    void calcStats(const Tree &tree, Stats &s) const
	    {
		AreaType overlap = 0;

		// calculate overlap
		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    for(unsigned int cj = ci+1; cj < this->childnum; cj++)
		    {
			assert(ci != cj);
			overlap += children[ci].getMBR(tree).getIntersectingArea(children[cj].getMBR(tree));
		    }
		}

		// calculate "outter" space

		AreaType waste = this->nodeMBR.getArea();
    		Rect rup;
		rup.setInfinite();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    Rect rnew;
		    rnew.intersectRects(rup, children[ci].getMBR(tree));

		    if (!rup.valid()) { // first rectangle
			waste -= rnew.getArea();
		    }
		    else { // subtract the enlargement
			waste -= rnew.getArea() - rup.getArea();
		    }

		    rup = rnew;
		}

		assert(waste >= 0);
		
		// add up stats
		LevelStats &ls = s.level.at(this->level);

		ls.nodes++;
		ls.children += this->childnum;
		ls.unusedchildren += pagesize - this->childnum;
		ls.bytes += sizeof(*this);
		ls.unusedbytes += (pagesize - this->childnum) * sizeof(NodeData);
		ls.overlap += overlap;
		ls.wastearea += waste;

		if (this->level > 0)
		{
		    // descend into child rectangles
		    const InnerNode *in = reinterpret_cast<const InnerNode*>(this);

		    for(unsigned int ci = 0; ci < in->childnum; ci++)
		    {
			NodeHead *cn = tree.pageman.getPage(in->children[ci].page);

			if (cn->level == 0)
			    static_cast<LeafNode*>(cn)->calcStats(tree, s);
			else
			    static_cast<InnerNode*>(cn)->calcStats(tree, s);
		    }
		}
    	    }

	    /// calculate the sum of all overlaps of this node and all children
	    AreaType	calcOverlap(const Tree &tree) const
	    {
		AreaType overlap = 0;
    
		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    for(unsigned int cj = ci+1; cj < this->childnum; cj++)
		    {
			assert(ci != cj);
			overlap += children[ci].getMBR(tree).getIntersectingArea(children[cj].getMBR(tree));
		    }
		}

		if (this->level > 0)
		{
		    // descend into child rectangles
		    const InnerNode *in = reinterpret_cast<const InnerNode*>(this);

		    for(unsigned int ci = 0; ci < in->childnum; ci++)
		    {
			NodeHead *cn = tree.pageman.getPage(in->children[ci].page);

			if (cn->level == 0)
			    overlap += static_cast<LeafNode*>(cn)->calcOverlap(tree);
			else
			    overlap += static_cast<InnerNode*>(cn)->calcOverlap(tree);
		    }
		}
    
		return overlap;
	    }
	
	    /// calculate the sum of the "wasted" space of this node and all children
	    AreaType	calcWasteArea(const Tree &tree) const
	    {
		AreaType waste = this->nodeMBR.getArea();
    		Rect rup;
		rup.setInfinite();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    Rect rnew;
		    rnew.intersectRects(rup, children[ci].getMBR(tree));

		    if (!rup.valid()) { // first rectangle
			waste -= rnew.getArea();
		    }
		    else { // subtract the enlargement
			waste -= rnew.getArea() - rup.getArea();
		    }

		    rup = rnew;
		}

		assert(waste >= 0);

		if (this->level > 0)
		{
		    // descend into child rectangles
		    const InnerNode *in = reinterpret_cast<const InnerNode*>(this);

		    for(unsigned int ci = 0; ci < in->childnum; ci++)
		    {
			NodeHead *cn = tree.pageman.getPage(in->children[ci].page);

			if (cn->level == 0)
			    waste += static_cast<LeafNode*>(cn)->calcWasteArea(tree);
			else
			    waste += static_cast<InnerNode*>(cn)->calcWasteArea(tree);
		    }
		}
    
		return waste;
	    }

	    // *** Insert and Reinsert functions

	    /// insert a new child rectangle in this inner node. if there is no
	    /// more room, invoke splitNode. returns true if the tree was adjusted.
	    bool	insertChild(Tree &tree, pageid_t mypageid, const NodeData &newdata, std::stack<pageid_t>& path, char *overflowedLevel)
	    {
		if (this->childnum < pagesize) // still room for one more?
		{
		    // fill in last child
		    this->children[this->childnum] = newdata;
		    this->childnum++;

		    // check if we need to adjust the rectangle path
		    if (not this->nodeMBR.containsRect(newdata.getMBR(tree)))
		    {
			this->nodeMBR.combineRect(newdata.getMBR(tree));
	    
			// and propagate the changes upward
			if (not path.empty())
			{
			    pageid_t parentpage = path.top();
			    path.pop();

			    NodeHead *parentnode = tree.pageman.getPage(parentpage);
			    assert(parentnode->level != 0);

			    static_cast<InnerNode*>(parentnode)->adjustTree(tree, parentpage, *this, mypageid, path);
			}
			return true;
		    }
		    else
			return false; // tree not adjusted
		}
#if defined(R_STAR_TREE)
		else if (!path.empty() && overflowedLevel[this->level] == 0 && tree.reinsertnum > 0)
		{
		    //std::cerr << "Reinsert on level " << level << "\n";
		    overflowedLevel[this->level] = 1;

		    reinsertData(tree, mypageid, newdata, path, overflowedLevel);

		    return true;
		}
#endif
		else
		{
		    // we have to split this node.
		    NodeType *nn;
		    pageid_t nn_page;

		    splitNode(tree, newdata, &nn, nn_page);

		    if (path.empty())
		    {
			// if we just split the root, then we have to create a new root
			// containing the two newly created nodes.

			//std::cerr << "Split Root\n";

			tree.rootpage = tree.pageman.new_innerpage();
			InnerNode *root = static_cast<InnerNode*>( tree.pageman.getPage(tree.rootpage) );

			root->initializeRoot();

			tree.levels++;
			root->level = this->level + 1;
			root->childnum = 2;

			assert(root->level == tree.levels);

			root->children[0].rect = this->nodeMBR;
			root->children[0].page = mypageid;

			root->children[1].rect = nn->nodeMBR;
			root->children[1].page = nn_page;

			root->nodeMBR.combineRects(this->nodeMBR, nn->nodeMBR);
		    }
		    else
		    {
			// else we have to insert nn into the parent page and propagate the
			// changes to the two nodeMBR upward

			//std::cerr << "Split\n";

			pageid_t parentpage = path.top();
			path.pop();

			NodeHead *parentnode = tree.pageman.getPage(parentpage);
			assert(parentnode->level != 0);

			static_cast<InnerNode*>(parentnode)->adjustTreeSplit(tree, parentpage,
									     *this, mypageid,
									     *nn, nn_page, path, overflowedLevel);
		    }

		    return true;
		}
	    }

	    /// adjust the rectangles in the path upward
	    void	adjustTree(Tree &tree, pageid_t mypageid, const NodeHead& adj, pageid_t adjpage, std::stack<pageid_t>& path)
	    {
		// find child number of the adj node.
		unsigned int adjchild;
		for(adjchild = 0; adjchild < this->childnum; adjchild++)
		{
		    if (this->children[adjchild].page == adjpage) break;
		}

		assert(adjchild < this->childnum);

		this->children[adjchild].rect = adj.nodeMBR;

		// this node's MBR needs recalculation if the newly inserted child's
		// rectangle is not contained (thus the MBR will grow)

		// we dont need any pre-calculation because the combineRect() takes just as
		// long as the containsRect().
		this->nodeMBR.combineRect(adj.nodeMBR);

		// and propagate the changes upward
		if (not path.empty())
		{
		    pageid_t parentpage = path.top();
		    path.pop();

		    NodeHead *parentnode = tree.pageman.getPage(parentpage);
		    assert(parentnode->level != 0);

		    static_cast<InnerNode*>(parentnode)->adjustTree(tree, parentpage, *this, mypageid, path);
		}
	    }

	    /// after a split, insert the new node NN into this page and adjust the
	    /// MBRs upward.
	    void	adjustTreeSplit(Tree &tree, pageid_t mypageid, const NodeHead &adjN, pageid_t adjNpage, const NodeHead &adjNN, pageid_t adjNNpage, std::stack<pageid_t>& path, char *overflowedLevel)
	    {
		// find child number of the adjN node.
		unsigned int adjchild;
		for(adjchild = 0; adjchild < this->childnum; adjchild++)
		{
		    if (this->children[adjchild].page == adjNpage) break;
		}

		assert(adjchild < this->childnum);

		// the nodeMBR of N most always grows smaller during a split. we always
		// recalculate this nodeMBR.
    
		this->children[adjchild].rect = adjN.nodeMBR;

		this->nodeMBR.setInfinite();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		    this->nodeMBR.combineRect(this->children[ci].getMBR(tree));

		// use the usual insert method to put the new node NN into this page.
		InnerNodeData newdata(adjNN.nodeMBR, adjNNpage);
		bool adjustedTree = insertChild(tree, mypageid, newdata, path, overflowedLevel);

		// if insertChild hasnt adjusted the tree, then we need to propagate the
		// changes upward
		if (!adjustedTree and not path.empty())
		{
		    pageid_t parentpage = path.top();
		    path.pop();

		    NodeHead *parentnode = tree.pageman.getPage(parentpage);
		    static_cast<InnerNode*>(parentnode)->adjustTreeReinsert(tree, parentpage, *this, mypageid, path);
		}
	    }

	    /// adjust the parent nodeMBR after a reinsert was performed on the
	    /// child (but before the actual reinsertion calls)
	    void	adjustTreeReinsert(Tree &tree, pageid_t mypageid, const NodeHead &adj, pageid_t adjpage,std::stack<pageid_t>& path)
	    {
		// find child number of the adjchild node.
		unsigned int adjci;
		for(adjci = 0; adjci < this->childnum; adjci++)
		{
		    if (this->children[adjci].page == adjpage) break;
		}

		assert(adjci < this->childnum);

		// the nodeMBR of N most always grows smaller during a reinsert. we always
		// recalculate this nodeMBR.
    
		this->children[adjci].rect = adj.nodeMBR;

		this->nodeMBR.setInfinite();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		    this->nodeMBR.combineRect(this->children[ci].getMBR(tree));

		// propagate the changes upward
		if (not path.empty())
		{
		    pageid_t parentpage = path.top();
		    path.pop();

		    NodeHead *parentnode = tree.pageman.getPage(parentpage);
		    static_cast<InnerNode*>(parentnode)->adjustTreeReinsert(tree, parentpage, *this, mypageid, path);
		}
	    }

	    /// R*-Tree forced reinsert of a number of rectangles from this node's elements
	    void	reinsertData(Tree &tree, pageid_t mypageid, const NodeData &newdata, std::stack<pageid_t>& path, char *overflowedLevel)
	    {
		assert(this->childnum == pagesize);

		std::vector<ReinsertDist> distlist(this->childnum+1);

		Point mycenter = this->nodeMBR.getCenter();

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    distlist[ci].childid = ci;

		    distlist[ci].distance = mycenter.getDistanceSquare( this->children[ci].getMBR(tree).getCenter() );
		    assert(distlist[ci].distance >= 0);
		}

		distlist[this->childnum].childid = this->childnum;
		distlist[this->childnum].distance = mycenter.getDistanceSquare( newdata.getMBR(tree).getCenter() );
		assert(distlist[this->childnum].distance >= 0);

		// sort by ascending order of distance
		std::sort(distlist.begin(), distlist.end(), ReinsertDist::order_distance);

		// the first (childnum+1)-reinsertnum entries are kept.
		unsigned int keptnum = (this->childnum+1) - tree.reinsertnum;

		// remove the following entries into a temporary space.
		std::vector<NodeData> reinsertLater(tree.reinsertnum);

		for(unsigned int i = 0; i < tree.reinsertnum; i++)
		{
		    if (distlist[keptnum+i].childid < this->childnum)
		    {
			reinsertLater[i] = this->children[ distlist[keptnum+i].childid ];
		    }
		    else
		    {
			reinsertLater[i] = newdata;
		    }
		}

		// sort the first reinsertnum entries and rearrange the children
		std::sort(distlist.begin(), distlist.begin() + keptnum, ReinsertDist::order_childid);

		this->nodeMBR.setInfinite();

		for(unsigned int ci = 0; ci < keptnum; ci++)
		{
		    if (distlist[ci].childid == ci)
		    {
			this->nodeMBR.combineRect(this->children[ci].getMBR(tree));
			continue;
		    }

		    if (distlist[ci].childid < this->childnum)
		    {
			this->children[ci] = this->children[ distlist[ci].childid ];
		    }
		    else
		    {
			this->children[ci] = newdata;
		    }

		    this->nodeMBR.combineRect(this->children[ci].getMBR(tree));
		}

		this->childnum = keptnum;

		// adjust the tree upward
		assert(!path.empty());
		{
		    pageid_t parentpage = path.top();
		    path.pop();

		    NodeHead *parentnode = tree.pageman.getPage(parentpage);
		    static_cast<InnerNode*>(parentnode)->adjustTreeReinsert(tree, parentpage, *this, mypageid, path);
		}

		// reinsert the removed children at maximum the same level
		for(unsigned int ri = 0; ri < tree.reinsertnum; ri++)
		{
		    tree.reinsertRect(reinsertLater[ri], this->level, overflowedLevel);
		}
	    }

	    /// delete the child rectangle at position ci, used by condenseTree
	    void	deleteChild(const Tree &tree, unsigned int ci)
	    {
		assert(ci < this->childnum);

		// rearrage the children if needed
		if (this->childnum > 0 && ci+1 != this->childnum)
		{
		    this->children[ci] = this->children[this->childnum-1];
		}

		this->childnum--;
    
		// recalculate the nodeMBR
		recalcChildrenMBR(tree, this->nodeMBR);
	    }

	    // *** Functions for splitting a node
	
	    /// split this node into two nodes of the same type, thus creating a
	    /// new one which is placed into nndest.
	    void	splitNode(Tree &tree, const NodeData &newdata, NodeType** nndest, pageid_t &nn_page)
	    {
		// create two groups of children rectangles
		std::vector<unsigned int> group1, group2;

#if defined(R_TREE_LINEAR) || defined(R_TREE_QUADRATIC)

		splitNodeMakeGroups(tree, newdata.getMBR(tree), group1, group2);

#elif defined(R_STAR_TREE)

		splitNodeMakeGroupsRStar(tree, newdata.getMBR(tree), group1, group2);

#endif	
		// create two new nodes on the same level and write the groups of children
		// into them. the new node n is only temporary and will be copied to this.
    
		nn_page = tree.pageman.newPage( sizeof(NodeType) );
    
		NodeType *n = new NodeType();
		NodeType *nn = static_cast<NodeType*>(tree.pageman.getPage(nn_page));

		n->initializeSibling(this->level);
		nn->initializeSibling(this->level);

		for(unsigned int gi = 0; gi < group1.size(); gi++)
		{
		    unsigned int ci = group1[gi];

		    if (ci < this->childnum)
		    {
			n->children[gi] = this->children[ci];
		    }
		    else
		    {
			// insert new rectangle into this node
			n->children[gi] = newdata;
		    }
		    n->nodeMBR.combineRect(n->children[gi].getMBR(tree));
		}
		n->childnum = static_cast<unsigned short>(group1.size());
    
		for(unsigned int gi = 0; gi < group2.size(); gi++)
		{
		    unsigned int ci = group2[gi];

		    if (ci < this->childnum)
		    {
			nn->children[gi] = this->children[ci];
		    }
		    else
		    {
			// insert new rectangle into this node
			nn->children[gi] = newdata;
		    }
		    nn->nodeMBR.combineRect(nn->children[gi].getMBR(tree));
		}
		nn->childnum = static_cast<unsigned short>(group2.size());

		// copy node n into this one
		*this = *n;
		delete n;

		*nndest = nn;
	    }

	    /// separate the rectangles of this node into two groups using the
	    /// quadratic algorithm
	    void		splitNodeMakeGroups(const Tree &tree, const Rect &newrect, std::vector<unsigned int> &group1, std::vector<unsigned int> &group2) const
	    {
		unsigned int seed1, seed2;

#if defined(R_TREE_LINEAR)

		pickSeedsLinear(tree, newrect, seed1, seed2);

#elif defined(R_TREE_QUADRATIC)

		pickSeedsQuadratic(tree, newrect, seed1, seed2);

#else
		assert(0);
#endif

		assert(seed1 <= this->childnum);
		assert(seed2 <= this->childnum);
		assert(seed1 != seed2);

		group1.push_back(seed1);
		group2.push_back(seed2);

		// char mask used to mark already assigned children
		char cmask[pagesize+1];
		memset(cmask, 0, pagesize+1);

		cmask[seed1] = 1;
		cmask[seed2] = 1;

		// min bounding rectangles of the two groups
		Rect mbr1 = (seed1 < this->childnum) ? children[seed1].getMBR(tree) : newrect;
		Rect mbr2 = (seed2 < this->childnum) ? children[seed2].getMBR(tree) : newrect;

		unsigned int num_ungrouped = (this->childnum + 1) - 2;

		unsigned int min_pageload = static_cast<unsigned int>(pagesize * 0.5);
    
		while(num_ungrouped > 0)
		{
		    // check if either group has so few entries that the rest must be
		    // assigned to it
		    if (group1.size() + num_ungrouped <= min_pageload)
		    {
			// assign rest of children to group1.
			for(unsigned int ci = 0; ci < this->childnum; ci++)
			{
			    if (cmask[ci]) continue;

			    group1.push_back(ci);
			    cmask[ci] = 1;
			    num_ungrouped--;
			}
			if (!cmask[this->childnum])
			{
			    group1.push_back(this->childnum);
			    cmask[this->childnum] = 1;
			    num_ungrouped--;
			}
			break;
		    }
		    else if (group2.size() + num_ungrouped <= min_pageload)
		    {
			// assign rest of children to group2.
			for(unsigned int ci = 0; ci < this->childnum; ci++)
			{
			    if (cmask[ci]) continue;

			    group2.push_back(ci);
			    cmask[ci] = 1;
			    num_ungrouped--;
			}
			if (!cmask[this->childnum])
			{
			    group2.push_back(this->childnum);
			    cmask[this->childnum] = 1;
			    num_ungrouped--;
			}
			break;
		    }
		    else
		    {
			// quadratic pickNext Algorithm: Select one remaining entry for
			// classification in a group.
	    
			// for all ungrouped rectangles, calculate the increase of the mbr
			// if it was put into either group. select that rectangle which has
			// the greatest difference.

			unsigned int childsel = std::numeric_limits<unsigned int>::max();
			AreaType childmaxdiff = std::numeric_limits<AreaType>::min();

			AreaType childsel_inc1 = std::numeric_limits<AreaType>::min(),
			    childsel_inc2 = std::numeric_limits<AreaType>::min();
	    
			Rect tr_group1, tr_group2;
	    
			AreaType area_mbr1 = mbr1.getArea();
			AreaType area_mbr2 = mbr2.getArea();

			for(unsigned int ci = 0; ci < this->childnum; ci++)
			{
			    if (cmask[ci]) continue;

			    tr_group1.combineRects(mbr1, children[ci].getMBR(tree));
			    tr_group2.combineRects(mbr2, children[ci].getMBR(tree));

			    assert(tr_group1.getArea() >= area_mbr1);
			    assert(tr_group2.getArea() >= area_mbr2);

			    AreaType increase1 = tr_group1.getArea() - area_mbr1;
			    AreaType increase2 = tr_group2.getArea() - area_mbr2;
		
			    AreaType diff = std::max(increase1 - increase2,
						     increase2 - increase1);
		
			    if (childmaxdiff < diff)
			    {
				childmaxdiff = diff;
				childsel = ci;
				childsel_inc1 = increase1;
				childsel_inc2 = increase2;
			    }
			}
			{ // and last but not least the new rectangle

			    if (!cmask[this->childnum])
			    {
				tr_group1.combineRects(mbr1, newrect);
				tr_group2.combineRects(mbr2, newrect);

				AreaType increase1 = tr_group1.getArea() - area_mbr1;
				AreaType increase2 = tr_group2.getArea() - area_mbr2;
		
				AreaType diff = std::max(increase1 - increase2,
							 increase2 - increase1);
		
				if (childmaxdiff < diff)
				{
				    childmaxdiff = diff;
				    childsel = this->childnum;
				    childsel_inc1 = increase1;
				    childsel_inc2 = increase2;
				}
			    }
			}
	    
			assert(childsel <= this->childnum);

			// childsel is the selected child rectangle. figure out the right group

			int group = 0;
			if (childsel_inc1 < childsel_inc2)
			{
			    group = 1;
			}
			else if (childsel_inc2 < childsel_inc1)
			{
			    group = 2;
			}
			// break ties by adding the entry to the group with smaller area
			else if (area_mbr1 < area_mbr2)
			{
			    group = 1;
			}
			else if (area_mbr2 < area_mbr1)
			{
			    group = 2;
			}
			// break further ties by putting it into the one with fewer rects
			else if (group1.size() < group2.size())
			{
			    group = 1;
			}
			else if (group2.size() < group1.size())
			{
			    group = 2;
			}
			// and last choose any
			else
			{
			    group = 1 + (childsel % 2);
			}

			// put the child number into the right group and enlarge the group MBR.
			if (group == 1)
			{
			    group1.push_back(childsel);
		
			    if (childsel < this->childnum)
				mbr1.combineRect(children[childsel].getMBR(tree));
			    else
				mbr1.combineRect(newrect);
			}
			else if (group == 2)
			{
			    group2.push_back(childsel);
		
			    if (childsel < this->childnum)
				mbr2.combineRect(children[childsel].getMBR(tree));
			    else
				mbr2.combineRect(newrect);
			}
			else {
			    assert(0);
			}

			cmask[childsel] = 1;
			num_ungrouped--;
		    }
		}

		assert(group1.size() + group2.size() == static_cast<unsigned int>(this->childnum) + 1);
		assert(group1.size() >= pageminfill);
		assert(group2.size() >= pageminfill);

#if 0
		std::cerr << "selected groups: " << std::endl;
		std::cerr << "g1:";
		for(unsigned int i = 0; i < group1.size(); i++)
		    std::cerr << " " << group1[i];
		std::cerr << std::endl;
		std::cerr << "g2:";
		for(unsigned int i = 0; i < group2.size(); i++)
		    std::cerr << " " << group2[i];
		std::cerr << std::endl;
#endif
	    }
   
	    /// pick the two group seeds using the quadratic algorithm
	    void		pickSeedsQuadratic(const Tree &tree, const Rect &newrect, unsigned int &seed1, unsigned int &seed2) const
	    {
		Rect rt;
		AreaType max_inefficiency = std::numeric_limits<AreaType>::min();

		// for each pair of entries E1 and E2
		for(unsigned int i = 0; i < this->childnum; i++)
		{
		    AreaType areaE1 = children[i].getMBR(tree).getArea();

		    for(unsigned int j = i+1; j < this->childnum; j++)
		    {
			rt.combineRects(children[i].getMBR(tree), children[j].getMBR(tree));

			AreaType d = rt.getArea() - areaE1 - children[j].getMBR(tree).getArea();

			if (d > max_inefficiency)
			{
			    max_inefficiency = d;
			    seed1 = i;
			    seed2 = j;
			}
		    }

		    // compare with the new rectangle
		    {
			rt.combineRects(children[i].getMBR(tree), newrect);

			AreaType d = rt.getArea() - areaE1 - newrect.getArea();

			if (d > max_inefficiency)
			{
			    max_inefficiency = d;
			    seed1 = i;
			    seed2 = this->childnum;
			}
		    }
		}
	    }
    
	    /// pick the two group seeds using the linear algorithm
	    void		pickSeedsLinear(const Tree &tree, const Rect &newrect, unsigned int &seed1, unsigned int &seed2) const
	    {
		// Find extreme rectangles along all dimensions

		// x-Axis

		CoordType max_x1, min_x2;
		unsigned int max_x1_ci, min_x2_ci;

		CoordType min_x1, max_x2;

		double normalized_x;

		{
		    // initialize max and min with the new rectangle
		    min_x1 = max_x1 = newrect.x1;
		    min_x2 = max_x2 = newrect.x2;
		    max_x1_ci = this->childnum;
		    min_x2_ci = this->childnum;

		    for(unsigned int ci = 0; ci < this->childnum; ci++)
		    {
			if (max_x1 < children[ci].getMBR(tree).x1)
			{
			    max_x1 = children[ci].getMBR(tree).x1;
			    max_x1_ci = ci;
			}
			min_x1 = std::min(min_x1, children[ci].getMBR(tree).x1);

			if (min_x2 > children[ci].getMBR(tree).x2)
			{
			    min_x2 = children[ci].getMBR(tree).x2;
			    min_x2_ci = ci;
			}
			max_x2 = std::min(max_x2, children[ci].getMBR(tree).x2);
		    }

		    CoordType separation = max_x1 - min_x2;

		    CoordType width = max_x2 - min_x1;
		    if (width <= 0) width = 1;

		    normalized_x = static_cast<double>(separation) / static_cast<double>(width);
		}

		// y-Axis

		CoordType max_y1, min_y2;
		unsigned int max_y1_ci, min_y2_ci;

		CoordType min_y1, max_y2;

		double normalized_y;

		{
		    // initialize max and min with the new rectangle
		    min_y1 = max_y1 = newrect.y1;
		    min_y2 = max_y2 = newrect.y2;
		    max_y1_ci = this->childnum;
		    min_y2_ci = this->childnum;

		    for(unsigned int ci = 0; ci < this->childnum; ci++)
		    {
			if (max_y1 < children[ci].getMBR(tree).y1)
			{
			    max_y1 = children[ci].getMBR(tree).y1;
			    max_y1_ci = ci;
			}
			min_y1 = std::min(min_y1, children[ci].getMBR(tree).y1);

			if (min_y2 > children[ci].getMBR(tree).y2)
			{
			    min_y2 = children[ci].getMBR(tree).y2;
			    min_y2_ci = ci;
			}
			max_y2 = std::min(max_y2, children[ci].getMBR(tree).y2);
		    }

		    CoordType separation = max_y1 - min_y2;

		    CoordType width = max_y2 - min_y1;
		    if (width <= 0) width = 1;

		    normalized_y = static_cast<double>(separation) / static_cast<double>(width);
		}

		if (normalized_y > normalized_x)
		{
		    // choose the extreme pair of the y-Axis
		    max_x1_ci = max_y1_ci;
		    min_x2_ci = min_y2_ci;
		}
		else {
		    // choose the extreme pair of the x-Axis
		}

		// break collision
		if (max_x1_ci == min_x2_ci)
		{
		    if (min_x2_ci == 0) min_x2_ci++;
		    else min_x2_ci--;
		}

		seed1 = max_x1_ci;
		seed2 = min_x2_ci;
	    }
    
	    /// data struct to order computed distributions
	    struct SplitDist
	    {
		unsigned int 	childid;
		Rect		mbr;

		static inline bool cmp_axis1_lower(const SplitDist &a, const SplitDist &b)
		{
		    return a.mbr.x1 < b.mbr.x1;
		}
		static inline bool cmp_axis1_upper(const SplitDist &a, const SplitDist &b)
		{
		    return a.mbr.x2 < b.mbr.x2;
		}
		static inline bool cmp_axis2_lower(const SplitDist &a, const SplitDist &b)
		{
		    return a.mbr.y1 < b.mbr.y1;
		}
		static inline bool cmp_axis2_upper(const SplitDist &a, const SplitDist &b)
		{
		    return a.mbr.y2 < b.mbr.y2;
		}

		typedef bool (*cmpfunc_t)(const SplitDist &a, const SplitDist &b);

		static inline cmpfunc_t get_cmpfunc(unsigned int axis, unsigned int lower_upper)
		{
		    if (axis == 0) {
			if (lower_upper == 0) return cmp_axis1_lower;
			if (lower_upper == 1) return cmp_axis1_upper;
		    }
		    if (axis == 1) {
			if (lower_upper == 0) return cmp_axis2_lower;
			if (lower_upper == 1) return cmp_axis2_upper;
		    }
		    assert(0);
		    return cmp_axis1_lower;
		}
	    };

	    /// separate the rectangles of this node into two groups using the
	    /// algorithm described in the R*-Tree paper.
	    void		splitNodeMakeGroupsRStar(const Tree &tree, const Rect &newrect, std::vector<unsigned int> &group1, std::vector<unsigned int> &group2) const
	    {
		assert(this->childnum == pagesize);

		std::vector<SplitDist> distlower(this->childnum+1);
		std::vector<SplitDist> distupper(this->childnum+1);

		for(unsigned int ci = 0; ci < this->childnum; ci++)
		{
		    distlower[ci].childid = ci;
		    distlower[ci].mbr = children[ci].getMBR(tree);

		    distupper[ci].childid = ci;
		    distupper[ci].mbr = children[ci].getMBR(tree);
		}

		distlower[this->childnum].childid = this->childnum;
		distlower[this->childnum].mbr = newrect;

		distupper[this->childnum].childid = this->childnum;
		distupper[this->childnum].mbr = newrect;

		// Algorithm chooseSplitAxis

		AreaType minMargin = std::numeric_limits<AreaType>::max();
		int splitAxis = -1;
		int splitIndex = -1;

		unsigned int distnum = this->childnum - 2*pageminfill + 2;
    
		{
		    // sort the two arrays by x
		    std::sort(distlower.begin(), distlower.end(), SplitDist::cmp_axis1_lower);
		    std::sort(distupper.begin(), distupper.end(), SplitDist::cmp_axis1_upper);

		    Rect bb_lower_group1, bb_upper_group1;
		    Rect bb_lower_group2, bb_upper_group2;

		    AreaType margin_lower = 0;
		    AreaType margin_upper = 0;

		    // for each of the M-2m+2 distributions, calculate the margin-values
		    for(unsigned int di = 0; di < distnum; di++)
		    {
			bb_lower_group1.setInfinite();
			bb_upper_group1.setInfinite();

			unsigned int listsplit = pageminfill + di;

			for(unsigned int i = 0; i < listsplit; i++)
			{
			    bb_lower_group1.combineRect(distlower[i].mbr);
			    bb_upper_group1.combineRect(distupper[i].mbr);
			}

			bb_lower_group2.setInfinite();
			bb_upper_group2.setInfinite();

			for(unsigned int i = listsplit; i < static_cast<unsigned int>(this->childnum+1); i++)
			{
			    bb_lower_group2.combineRect(distlower[i].mbr);
			    bb_upper_group2.combineRect(distupper[i].mbr);
			}

			// compute S, the sum of all margin-values
			margin_lower += bb_lower_group1.getMargin() + bb_lower_group2.getMargin();
			margin_upper += bb_upper_group1.getMargin() + bb_upper_group2.getMargin();
		    }
	
		    AreaType margin = std::min(margin_lower, margin_upper);

		    // find the minimum margin value
		    if (minMargin > margin)
		    {
			minMargin = margin;
			splitAxis = 0;
			splitIndex = (margin_lower < margin_upper) ? 0 : 1;
		    }
		}
		{ // same as above but this time sorted by y
		    // sort the two arrays by y
		    std::sort(distlower.begin(), distlower.end(), SplitDist::cmp_axis2_lower);
		    std::sort(distupper.begin(), distupper.end(), SplitDist::cmp_axis2_upper);

		    Rect bb_lower_group1, bb_upper_group1;
		    Rect bb_lower_group2, bb_upper_group2;

		    AreaType margin_lower = 0;
		    AreaType margin_upper = 0;

		    // for each of the M-2m+2 distributions, calculate the margin-values
		    for(unsigned int di = 0; di < distnum; di++)
		    {
			bb_lower_group1.setInfinite();
			bb_upper_group1.setInfinite();

			unsigned int listsplit = pageminfill + di;

			for(unsigned int i = 0; i < listsplit; i++)
			{
			    bb_lower_group1.combineRect(distlower[i].mbr);
			    bb_upper_group1.combineRect(distupper[i].mbr);
			}

			bb_lower_group2.setInfinite();
			bb_upper_group2.setInfinite();

			for(unsigned int i = listsplit; i < static_cast<unsigned int>(this->childnum+1); i++)
			{
			    bb_lower_group2.combineRect(distlower[i].mbr);
			    bb_upper_group2.combineRect(distupper[i].mbr);
			}

			// compute S, the sum of all margin-values
			margin_lower += bb_lower_group1.getMargin() + bb_lower_group2.getMargin();
			margin_upper += bb_upper_group1.getMargin() + bb_upper_group2.getMargin();
		    }
	
		    AreaType margin = std::min(margin_lower, margin_upper);
		    assert(margin >= 0);

		    // find the minimum margin value
		    if (minMargin > margin)
		    {
			minMargin = margin;
			splitAxis = 1;
			splitIndex = (margin_lower < margin_upper) ? 0 : 1;
		    }
		}

		// resort the distlist by the selected order
		std::sort(distlower.begin(), distlower.end(), SplitDist::get_cmpfunc(splitAxis, splitIndex));

		// Algorithm chooseSplitIndex

		unsigned int min_distrib = distnum;
		AreaType min_overlap = std::numeric_limits<AreaType>::max();
		AreaType min_area = std::numeric_limits<AreaType>::max();
		{
		    Rect bb_group1, bb_group2;

		    // for each of the M-2m+2 distributions, calculate the minimum overlap
		    for(unsigned int di = 0; di < distnum; di++)
		    {
			bb_group1.setInfinite();

			unsigned int listsplit = pageminfill + di;

			for(unsigned int i = 0; i < listsplit; i++)
			{
			    bb_group1.combineRect(distlower[i].mbr);
			}

			bb_group2.setInfinite();

			for(unsigned int i = listsplit; i < static_cast<unsigned int>(this->childnum+1); i++)
			{
			    bb_group2.combineRect(distlower[i].mbr);
			}

			AreaType overlap = bb_group1.getIntersectingArea(bb_group2);
			assert(overlap >= 0);

			if (min_overlap > overlap)
			{
			    min_distrib = di;
			    min_overlap = overlap;
			    min_area = bb_group1.getArea() + bb_group2.getArea();
			}
			// break ties by choosing the distribution with minimum area-value
			else if (min_overlap == overlap)
			{
			    if (bb_group1.getArea() + bb_group2.getArea() < min_area)
			    {
				min_distrib = di;
				min_overlap = overlap;
				min_area = bb_group1.getArea() + bb_group2.getArea();
			    }
			}
		    }
		    assert(min_distrib < distnum);
		}

		// put the selected distribution into the right groups
		{
		    unsigned int listsplit = pageminfill + min_distrib;

		    for(unsigned int i = 0; i < listsplit; i++)
		    {
			group1.push_back(distlower[i].childid);
		    }

		    for(unsigned int i = listsplit; i < static_cast<unsigned int>(this->childnum+1); i++)
		    {
			group2.push_back(distlower[i].childid);
		    }
		}

		assert(group1.size() + group2.size() == static_cast<unsigned int>(this->childnum) + 1);
		assert(group1.size() >= pageminfill);
		assert(group2.size() >= pageminfill);
	    }

	    /// condense a node if has too few entries
	    void		condenseTree(Tree &tree, pageid_t mypageid, std::stack<pageid_t>& toReinsert, std::queue<pageid_t>& path)
	    {
		if (path.empty())
		{
		    // condenseTree the root if it is not a leaf and has just one child
		    if (this->level != 0 && this->childnum == 1)
		    {
			InnerNode *in = reinterpret_cast<InnerNode*>(this);

			tree.rootpage = in->children[0].page;

			tree.levels--;

			tree.pageman.deletePage(mypageid);
		    }
		}
		else
		{
		    // find the InnerNode containing a child pointing to this node
		    pageid_t parentpage = path.front();
		    path.pop();

		    InnerNode *parentnode = static_cast<InnerNode*>(tree.pageman.getPage(parentpage));
		    assert(parentnode->level != 0);

		    // find the child position of this node
		    unsigned int ci;
		    for(ci = 0; ci < parentnode->childnum; ci++)
		    {
			if (parentnode->children[ci].page == mypageid) break;
		    }

		    assert(ci < parentnode->childnum);

		    if (this->childnum < pageminfill)
		    {
			// this node will be deleted and entries moved to other nodes on the same level
			parentnode->deleteChild(tree, ci);

			toReinsert.push(mypageid);
		    }
		    else
		    {
			// no need to delete this node, adapt the nodeMBR of our parent
			parentnode->children[ci].rect = this->nodeMBR;

			parentnode->recalcChildrenMBR(tree, parentnode->nodeMBR);
		    }

		    // propagate upward
		    parentnode->condenseTree(tree, parentpage, toReinsert, path);
		}
	    }
	};

	/// data struct of the elements found in each inner node
	struct InnerNodeData
	{
	public:
	    /// full children rectangle structure
	    Rect	rect;

	    /// page id of the child node
	    pageid_t	page;

	    /// noop constructor
	    inline InnerNodeData()
	    { }

	    /// initializing constructor
	    inline InnerNodeData(const Rect &r, pageid_t p)
		: rect(r), page(p)
	    { }

	    /// return the child node rectangle
	    inline const class Rect getMBR(const Tree &) const
	    { return rect; }
	};

	/// structure used to access fields of an inner node page of the R-Tree
	class InnerNode : public Node<InnerNodeData>
	{
	public:
	    // nothing further than Node's functions
	};

	/// data struct of the elements in the leaf nodes. it is based on the template parameter
	class LeafNodeData : public DataType
	{
	public:
	    LeafNodeData() : DataType()
	    { }

	    /// initialising constructor
	    LeafNodeData(const DataType &init) : _DataType(init)
	    { }

	    /// return the child node rectangle by calling the provided DataType Callback.
	    inline const class Rect getMBR(const Tree &tree) const
	    { return tree.DTCallback->getMBR(*static_cast<const DataType*>(this)); }
	};

	/// structure used to access fields of a leaf node page of the R-Tree
	class LeafNode : public Node<LeafNodeData>
	{
	public:
	    /// delete a data child from this leaf
	    void	deleteDataLeaf(Tree &tree, pageid_t mypageid, const LeafNodeData &rmdata, std::queue<pageid_t>& path)
	    {
		assert(this->level == 0);

		// find the entry that should be deleted
    
		unsigned int ci;
		for(ci = 0; ci < this->childnum; ci++)
		{
		    if (this->children[ci] == rmdata) break;
		}

		assert(ci < this->childnum);

		// rearrage the children if needed
		if (this->childnum > 0 && ci+1 != this->childnum)
		{
		    this->children[ci] = this->children[this->childnum-1];
		}

		this->childnum--;
    
		// recalculate the nodeMBR
		recalcChildrenMBR(tree, this->nodeMBR);

		// call condenseTree to propagte changes

		std::stack<pageid_t> toReinsert;
		condenseTree(tree, mypageid, toReinsert, path);

		// reinsert all child of the nodes which are going to be deleted
		while(!toReinsert.empty())
		{
		    pageid_t delpage = toReinsert.top();
		    toReinsert.pop();

		    NodeHead *delnode = tree.pageman.getPage(delpage);

		    if (delnode->level == 0)
		    {
			LeafNode *ln = static_cast<LeafNode*>(delnode);

			for(unsigned int ci = 0; ci < ln->childnum; ci++)
			{
			    char *overflowedLevel = static_cast<char*>(alloca(tree.levels+1));
			    memset(overflowedLevel, 0, tree.levels+1);

			    tree.reinsertRect(ln->children[ci], 0, overflowedLevel);
			}
		    }
		    else
		    {
			InnerNode *in = static_cast<InnerNode*>(delnode);

			for(unsigned int ci = 0; ci < in->childnum; ci++)
			{
			    char *overflowedLevel = static_cast<char*>(alloca(tree.levels+1));
			    memset(overflowedLevel, 0, tree.levels+1);

			    tree.reinsertRect(in->children[ci], in->level, overflowedLevel);
			}
		    }

		    tree.pageman.deletePage(delpage);
		}
	    }
	};

	// *** Methods used to reinsert data and inner pointers into the tree

	/// reinsert used by the R*-Tree algorithm. inserts a data element into the tree at maxlevel
	template <typename NodeData>
	void	reinsertRect(NodeData &newdata, unsigned int maxlevel, char *overflowedLevel)
	{
	    NodeHead *root = pageman.getPage(rootpage);

	    // choose the node in which to insert the new rectangle
	    std::stack<pageid_t> path;

	    path.push(rootpage);
	    class NodeHead *node;
	    if (root->level == 0)
		node = static_cast<LeafNode*>(root)->chooseSubtree(*this, newdata.getMBR(*this), maxlevel, path);
	    else
		node = static_cast<InnerNode*>(root)->chooseSubtree(*this, newdata.getMBR(*this), maxlevel, path);

	    assert(node);
	    assert(node->level == maxlevel);

	    pageid_t page = path.top(); // the last entry is the node.
	    path.pop();
	    static_cast<Node<NodeData>*>(node)->insertChild(*this, page, newdata, path, overflowedLevel);
	}

	/// poor man's page manager. used so that no pointers are saved in the R-Tree data
	class PageManager
	{
	public:
	    typedef std::vector<class InnerNode*> innerpages_t;
	    std::vector<class InnerNode*> innerpages;

	    typedef std::vector<class LeafNode*> leafpages_t;
	    std::vector<class LeafNode*> leafpages;

	    PageManager()
	    { }

	    PageManager(const PageManager &c)
	    {
		for(unsigned int i = 0; i < c.innerpages.size(); i++)
		    innerpages.push_back( new InnerNode(*c.innerpages[i]) );

		for(unsigned int i = 0; i < c.leafpages.size(); i++)
		    leafpages.push_back( new LeafNode(*c.leafpages[i]) );
	    }

	    PageManager& operator=(const PageManager &c)
	    {
		clear();

		for(unsigned int i = 0; i < c.innerpages.size(); i++)
		    innerpages.push_back( new InnerNode(*c.innerpages[i]) );

		for(unsigned int i = 0; i < c.leafpages.size(); i++)
		    leafpages.push_back( new LeafNode(*c.leafpages[i]) );

		return *this;
	    }
	
	    ~PageManager()
	    {
		for(unsigned int pi = 0; pi < innerpages.size(); pi++)
		    delete innerpages[pi];

		for(unsigned int pi = 0; pi < leafpages.size(); pi++)
		    delete leafpages[pi];
	    }

	    static const pageid_t leafmask = 0x80000000;

	    class NodeHead*	getPage(pageid_t pi) const
	    {
		if (pi & leafmask)
		{
		    assert((pi & ~leafmask) < leafpages.size());
		    assert(leafpages[(pi & ~leafmask)]);
		    return leafpages[(pi & ~leafmask)];
		}
		else
		{
		    assert(pi < innerpages.size());
		    assert(innerpages[pi]);
		    return innerpages[pi];
		}
	    }

	    pageid_t	new_innerpage()
	    {
		for(pageid_t pi = 0; pi < innerpages.size(); pi++)
		{
		    if (innerpages[pi] == static_cast<InnerNode*>(NULL))
		    {
			innerpages[pi] = new InnerNode;
			return pi;
		    }
		}

		innerpages.push_back(new InnerNode);
		return static_cast<pageid_t>(innerpages.size()-1);
	    }

	    /// create a new leaf page
	    pageid_t	new_leafpage()
	    {
		for(pageid_t pi = 0; pi < leafpages.size(); pi++)
		{
		    if (leafpages[pi] == static_cast<LeafNode*>(NULL))
		    {
			leafpages[pi] = new LeafNode;
			return pi | leafmask;
		    }
		}

		leafpages.push_back(new LeafNode);
		return static_cast<pageid_t>((leafpages.size()-1) | leafmask);
	    }

	    /// this is an evil workaround to get a new page of a generic type,
	    /// because we need that in the splitNode() function.
	    pageid_t newPage(unsigned int nodesize)
	    {
		assert( sizeof(InnerNode) != sizeof(LeafNode) );

		if (nodesize == sizeof(InnerNode))
		    return new_innerpage();
		else if (nodesize == sizeof(LeafNode))
		    return new_leafpage();
		else {
		    assert(0);
		    return new_innerpage();
		}
	    }

	    void deletePage(pageid_t pi)
	    {
		if (pi & leafmask)
		{
		    assert((pi & ~leafmask) < leafpages.size());
		    assert(leafpages[pi & ~leafmask]);
		    delete leafpages[pi & ~leafmask];
		    leafpages[pi & ~leafmask] = NULL;
		}
		else
		{
		    assert(pi < innerpages.size());
		    assert(innerpages[pi]);
		    delete innerpages[pi];
		    innerpages[pi] = NULL;
		}
	    }

	    void clear()
	    {
		for(unsigned int pi = 0; pi < innerpages.size(); pi++)
		    delete innerpages[pi];

		for(unsigned int pi = 0; pi < leafpages.size(); pi++)
		    delete leafpages[pi];

		innerpages.clear();
		leafpages.clear();
	    }
	};

    private:
	// *** Data of the R-Tree

	pageid_t	rootpage;

	PageManager	pageman;

	unsigned int 	levels;

	unsigned int 	entries;

	mutable DataTypeCallback	*DTCallback;

    public:
	// *** Operations on the R-Tree

	explicit Tree(DataTypeCallback *cb)
	{
	    rootpage = pageman.new_leafpage();

	    NodeHead *root = pageman.getPage(rootpage);
	    root->initializeRoot();

	    levels = 0;
	    entries = 0;
	    DTCallback = cb;

	    reinsertnum = pagesize - pageminfill;
	    assert(pageminfill + reinsertnum <= pagesize);
	}

	/// copy construct a tree, copies the callback
	Tree(const Tree &o)
	{
	    rootpage = o.rootpage;
	    pageman = o.pageman;

	    levels = o.levels;
	    entries = o.entries;
	    reinsertnum = o.reinsertnum;
	    DTCallback = o.DTCallback;
	}

	/// assigns the data of the rtree, does not copy the callback
	Tree& operator=(const Tree &o)
	{
	    rootpage = o.rootpage;
	    pageman = o.pageman;

	    levels = o.levels;
	    entries = o.entries;
	    reinsertnum = o.reinsertnum;

	    return *this;
	}

	/// reset the R-Tree by removing all elements
	void	clear()
	{
	    pageman.clear();

	    rootpage = pageman.new_leafpage();

	    NodeHead *root = pageman.getPage(rootpage);
	    root->initializeRoot();

	    levels = 0;
	    entries = 0;

	    assert(pageminfill + reinsertnum <= pagesize);
	}

	/// change the callback
	void	setCallback(DataTypeCallback *newcb)
	{
	    DTCallback = newcb;
	}

	/// change reinsert number
	void	setReinsertNum(int rn)
	{
	    reinsertnum = rn;
	    assert(pageminfill + reinsertnum <= pagesize);
	}
	
	/// insert function of the R-Tree
	void	insertRect(const DataRef newdata)
	{
	    assert(DTCallback->getMBR(newdata).valid());

	    NodeHead *root = pageman.getPage(rootpage);

	    // choose the leaf in which to insert the new rectangle
	    std::stack<pageid_t> path;

	    path.push(rootpage);
	    class NodeHead *_leaf;
	    if (root->level == 0) {
		_leaf = static_cast<class LeafNode*>(root)->chooseSubtree(*this, DTCallback->getMBR(newdata), 0, path);
	    }
	    else {
		_leaf = static_cast<class InnerNode*>(root)->chooseSubtree(*this, DTCallback->getMBR(newdata), 0, path);
	    }

	    assert(_leaf);
	    assert(_leaf->level == 0);
	    class LeafNode *leaf = static_cast<class LeafNode*>(_leaf);

	    // table of overflowed levels in the R*-Tree
	    char *overflowedLevel = static_cast<char*>(alloca(levels+1));
	    memset(overflowedLevel, 0, levels+1);

	    pageid_t leafpage = path.top(); // the last entry is the leaf.
	    path.pop();
	    leaf->insertChild(*this, leafpage, LeafNodeData(newdata), path, overflowedLevel);

	    entries++;
	}

	/// delete function of the R-Tree
	bool	deleteRect(const DataRef rmdata)
	{
	    assert(DTCallback->getMBR(rmdata).valid());
	    NodeHead *root = pageman.getPage(rootpage);

	    // find the leaf from which to delete the rectangle
	    std::queue<pageid_t> path;

	    class LeafNode *leaf;
	    if (root->level == 0) {
		leaf = static_cast<class LeafNode*>(root)->findLeaf(*this, LeafNodeData(rmdata), path);
	    }
	    else {
		leaf = static_cast<class InnerNode*>(root)->findLeaf(*this, LeafNodeData(rmdata), path);
	    }

	    path.push(rootpage);

	    if (!leaf) return false;

	    assert(leaf->level == 0);

	    pageid_t leafpage = path.front(); // the last entry is the leaf.
	    path.pop();

	    leaf->deleteDataLeaf(*this, leafpage, rmdata, path);

	    entries--;

	    return true;
	}

	/// write the rectangles out in the fig format. stop at a given level (0 are the leaves)
	void	writeFig(bool writefigheader, std::ostream &o, int maxlevel) const
	{
	    if (writefigheader)
		o << "#FIG 3.2\nPortrait\nCenter\nMetric\nA0\n100.00\nSingle\n-2\n10000 2\n";

	    // colors of the rtree levels

	    o << "0 32 #9E9E9E" << std::endl;
	    o << "0 33 #7CF22D" << std::endl;
	    o << "0 34 #1757C6" << std::endl;
	    o << "0 35 #F00000" << std::endl;
	    o << "0 36 #AD14AD" << std::endl;

	    NodeHead *root = pageman.getPage(rootpage);

#ifdef MOVIEMAKING
	    Rect rect = root->nodeMBR;

	    rect = Rect(602460, -5483624, 1500876, -4751257);

	    // output rectangle as fig polyline
	    o << "2 2 0 1 " << 32 + root->level + 1 << " 7 " << 100 - root->level << " -1 -1 0.000 0 0 -1 0 0 5\n"
	      << "       "
	      << rect.x1 << " " << rect.y1 << " "
	      << rect.x2 << " " << rect.y1 << " "
	      << rect.x2 << " " << rect.y2 << " "
	      << rect.x1 << " " << rect.y2 << " "
	      << rect.x1 << " " << rect.y1 << "\n";
#endif

	    if (root->level == 0)
		static_cast<LeafNode*>(root)->writeFig(*this, o, maxlevel);
	    else
		static_cast<InnerNode*>(root)->writeFig(*this, o, maxlevel);
	}

	/// save a serialized representation to the file
	void	saveSnapshot(std::ostream &s) const
	{
	    // first write a signature
	    unsigned int key = 0x34343434;
	    s.write(reinterpret_cast<char*>(&key), sizeof(key));

	    // *** write vital r-tree parameters
	    key = pagesize;
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));

	    key = sizeof(pageid_t);
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));

	    key = sizeof(DataType);
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));

	    key = sizeof(class InnerNode);
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));

	    key = sizeof(class LeafNode);
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));

	    // *** write global variables
	    s.write(reinterpret_cast<const char*>(&rootpage), sizeof(rootpage));

	    s.write(reinterpret_cast<const char*>(&levels), sizeof(levels));

	    s.write(reinterpret_cast<const char*>(&entries), sizeof(entries));

	    // *** write inner pages. valgrind will complain here, because we save
	    // *** memory which was uninitialized: the unused children in each page.
	    key = pageman.innerpages.size();
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));
    
	    for(unsigned int i = 0; i < pageman.innerpages.size(); i++)
	    {
		if (pageman.innerpages[i] == NULL) continue;

		key = i;
		s.write(reinterpret_cast<const char*>(&key), sizeof(key));

		s.write(reinterpret_cast<const char*>(pageman.innerpages[i]), sizeof(InnerNode));
	    }

	    // *** write leaf pages
	    key = pageman.leafpages.size();
	    s.write(reinterpret_cast<const char*>(&key), sizeof(key));
    
	    for(unsigned int i = 0; i < pageman.leafpages.size(); i++)
	    {
		if (pageman.leafpages[i] == NULL) continue;

		key = i;
		s.write(reinterpret_cast<const char*>(&key), sizeof(key));

		s.write(reinterpret_cast<const char*>(pageman.leafpages[i]), sizeof(LeafNode));
	    }
	}

	/// reload the serialized representation
	bool	loadSnapshot(std::istream &s)
	{
	    unsigned int key;
    
	    // read signature
	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != 0x34343434) return false;

	    // *** read vital r-tree parameters
	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != pagesize) return false;

	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != sizeof(pageid_t)) return false;

	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != sizeof(DataType)) return false;
    
	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != sizeof(class InnerNode)) return false;

	    if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
	    if (key != sizeof(class LeafNode)) return false;

	    // wipe tree
	    clear();

	    // *** read global variables

	    if (!s.read(reinterpret_cast<char*>(&rootpage), sizeof(rootpage))) return false;

	    if (!s.read(reinterpret_cast<char*>(&levels), sizeof(levels))) return false;

	    if (!s.read(reinterpret_cast<char*>(&entries), sizeof(entries))) return false;

	    std::cerr << "Loading R-Tree with " << entries << " entries: ";

	    // *** read inner pages
	    unsigned int len;

	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;
	    pageman.innerpages.resize(len);
    
	    for(unsigned int i = 0; i < len; i++)
	    {
		if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
		if (key >= len) return false;

		pageman.innerpages[key] = new InnerNode;
		if (!s.read(reinterpret_cast<char*>(pageman.innerpages[key]), sizeof(InnerNode))) return false;
	    }

	    std::cerr << len << " inner nodes and ";

	    // *** read leaf pages
	    if (!s.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;
	    pageman.leafpages.resize(len);
    
	    for(unsigned int i = 0; i < len; i++)
	    {
		if (!s.read(reinterpret_cast<char*>(&key), sizeof(key))) return false;
		if (key >= len) return false;

		pageman.leafpages[key] = new LeafNode;
		if (!s.read(reinterpret_cast<char*>(pageman.leafpages[key]), sizeof(LeafNode))) return false;
	    }
    
	    std::cerr << len << " leaf nodes.\n";

	    return true;
	}

	/// assertion of various tree invariants
	bool	testTree() const
	{
	    NodeHead *root = pageman.getPage(rootpage);

	    unsigned int testentries = 0;

	    if (root->level == 0) {
		return static_cast<class LeafNode*>(root)->checkNode(*this, levels, &testentries);
	    }
	    else {
		return static_cast<class InnerNode*>(root)->checkNode(*this, levels, &testentries);
	    }

	    assert(testentries == entries);
	}

	/// calculate the statistics of the tree
	void calcStats(Stats &s) const
	{
	    NodeHead *root = pageman.getPage(rootpage);

	    if (s.level.size() <= levels) {
		s.level.resize(levels+1);
	    }

	    if (root->level == 0) {
		static_cast<class LeafNode*>(root)->calcStats(*this, s);
	    }
	    else {
		static_cast<class InnerNode*>(root)->calcStats(*this, s);
	    }
	}
	/// calculate the sum of all overlaps in all levels
	AreaType	calcOverlap() const
	{
	    NodeHead *root = pageman.getPage(rootpage);

	    if (root->level == 0) {
		return static_cast<class LeafNode*>(root)->calcOverlap(*this);
	    }
	    else {
		return static_cast<class InnerNode*>(root)->calcOverlap(*this);
	    }
	}

	/// calculate the sum of all "wasted" areas in all levels
	AreaType	calcWasteArea() const
	{
	    NodeHead *root = pageman.getPage(rootpage);

	    if (root->level == 0) {
		return static_cast<class LeafNode*>(root)->calcWasteArea(*this);
	    }
	    else {
		return static_cast<class InnerNode*>(root)->calcWasteArea(*this);
	    }
	}

	/// return the number of elements in the R-Tree
	unsigned int size() const
	{ return entries; }

	/// check if the tree is empty
	bool	empty() const
	{ return (entries == 0); }

	/// query the R-Tree for all rectangles which intersect with the given
	/// search rectangle. call the template class for each of them.
	template <typename Visitor>
	void	queryRange(const Rect &range, Visitor &vis) const
	{
	    assert(range.valid());
	
	    std::stack<pageid_t> pagestack;
	    pagestack.push(rootpage);

	    while(!pagestack.empty())
	    {
		pageid_t page = pagestack.top();
		pagestack.pop();

		NodeHead *node = pageman.getPage(page);

		if (node->level == 0)
		{
		    LeafNode *leaf = static_cast<LeafNode*>(node);

		    for(unsigned int ci = 0; ci < leaf->childnum; ci++)
		    {
			if (leaf->children[ci].getMBR(*this).intersectsRect(range))
			{
			    if (!vis(static_cast<const Rect&>(leaf->children[ci].getMBR(*this)),
				     static_cast<DataType&>(leaf->children[ci])))
				return;
			}
		    }
		}
		else
		{
		    InnerNode *inner = static_cast<InnerNode*>(node);

		    for(unsigned int ci = 0; ci < inner->childnum; ci++)
		    {
			if (inner->children[ci].getMBR(*this).intersectsRect(range))
			{
			    pagestack.push(inner->children[ci].page);
			}
		    }
		}
	    }
	}

	/// class containing the data and ordering function of the nearest neighbor
	/// priority queue
	class NNPQData
	{
	public:
	    pageid_t	page;

	    AreaType	mindist;

	    NNPQData(pageid_t _page, AreaType _mindist)
		: page(_page), mindist(_mindist)
	    {
	    }

	    bool operator< (const class NNPQData &o) const
	    { return mindist < o.mindist; }
	};

	// *** Warning: This code was never tested! It was written then decided
	// *** that it could not be used for it's purpose.

	/// query the R-Tree for the nearest neighbor rectangles to a certain
	/// point. call the visitor template class for narrowing it down.
	template <typename NNVisitor>
	void	queryNearestNeighbor(const Point &point, NNVisitor &vis) const
	{
	    // put all rectangles into the priority queue ordering them by distance
	    // to the point, if the rectangle distance is greater then
	    // vis.mindistance then discard the subtree without entering it.

	    std::priority_queue<NNPQData, std::deque<NNPQData> > rectqueue;

	    rectqueue.push(NNPQData(rootpage, 0));

	    while(!rectqueue.empty())
	    {
		NNPQData rp = rectqueue.top();

		// if this rectangle's mindistance is larger then the currently
		// found neighbor, then break the loop.
		if (rp.mindist >= vis.mindistance) break;

		rectqueue.pop();

		// fetch the page
		NodeHead *node = pageman.getPage(rp.page);

		if (node->level == 0)
		{
		    LeafNode *leaf = static_cast<LeafNode*>(node);

		    for(unsigned int ci = 0; ci < leaf->childnum; ci++)
		    {
			AreaType md = point.getMinimumDistanceSquare(leaf->children[ci].getMBR(*this));

			if (md <= vis.mindistance)
			{
			    if (!vis(static_cast<const Rect&>(leaf->children[ci].getMBR(*this)),
				     md,
				     static_cast<DataType&>(leaf->children[ci])))
				return;
			}
		    }
		}
		else
		{
		    InnerNode *inner = static_cast<InnerNode*>(node);

		    for(unsigned int ci = 0; ci < inner->childnum; ci++)
		    {
			// push the all subrectangles onto the rectqueue
			AreaType md = point.getMinimumDistanceSquare(inner->children[ci].getMBR(*this));

			if (md <= vis.mindistance)
			    rectqueue.push( NNPQData(inner->children[ci].page, md) );
		    }
		}

	    }
	}
    };
};

/// print statistical stuff out
inline std::ostream& operator<< (std::ostream &o, const RTree::LevelStats &ls)
{
    return o << "nodes: " << ls.nodes << ", "
	     << "children: " << ls.children << ", "
	     << "unused: " << ls.unusedchildren << ", "
	     << "avgfill: " << (static_cast<double>(ls.children) / ls.nodes) << ", "
	     << "bytes: " << ls.bytes << ", "
	     << "unused: " << ls.unusedbytes << ", "
	     << "overlap: " << ls.overlap << ", "
	     << "wastearea: " << ls.wastearea;
}

/// add things up
inline RTree::LevelStats operator+ (const RTree::LevelStats &a, const RTree::LevelStats &b)
{
    RTree::LevelStats c;

    c += a;
    c += b;

    return c;
}

} // namespace VGServer

#endif // VGS_RTree_H
