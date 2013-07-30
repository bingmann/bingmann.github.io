// $Id: PythagorasTree.h 9 2006-08-08 08:29:15Z tb $

#include <math.h>

class PythagorasTree : public Picture
{
public:

    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect::ByCenter(0.0, 4.0, 9.0);
    }

    static inline Uint32 make_color(int index)
    {
	index += 17;
	return ((index * 23) & 0xFF) << 24
	    |  ((index * 7) & 0xFF) << 16
	    |  ((index * 19) & 0xFF) << 8
	    |  0x000000FF;
    }

    /// order or maxdepth of the displayed fractal
    int maxdepth;

    PythagorasTree() : Picture() {
	resetVariables();
    }

    virtual void resetVariables()
    {
	maxdepth = 10;
    }

    /// currenty depth
    int depth;

    /// draw one square of the pythagoras tree and call recursively for the
    /// next two adjacent squares.
    void drawBase(Canvas &canvas, Point a, Point b)
    {
	if (depth > maxdepth) return;
	depth++;

	double len = a.distance(b);
	double alpha = atan((b.y-a.y) / (b.x-a.x));
    
	if (a.x > b.x) alpha += M_PI;
    
	Point c ( b.x - len * sin(alpha),
		  b.y + len * cos(alpha) );

	Point d ( a.x - len * sin(alpha),
		  a.y + len * cos(alpha) );

	double mlen = sqrt(len * len / 2);

	Point e ( d.x + mlen * cos(alpha + M_PI / 4.0),
		  d.y + mlen * sin(alpha + M_PI / 4.0) );


	canvas.Line(a, b, make_color(depth));
	canvas.Line(b, c, make_color(depth));
	canvas.Line(c, d, make_color(depth));
	canvas.Line(d, a, make_color(depth));

	drawBase(canvas, d, e);
	drawBase(canvas, e, c);
	
	depth--;
    }

    virtual void drawPicture(Canvas &c)
    {
	c.Blank(0x00000000);

	depth = 0;

	drawBase(c,
		 Point(-1.0,0.0),
		 Point(+1.0,0.0));
    }
};
