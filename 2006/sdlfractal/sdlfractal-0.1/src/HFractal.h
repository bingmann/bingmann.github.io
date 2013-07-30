// $Id: HFractal.h 9 2006-08-08 08:29:15Z tb $

#include <math.h>

class HFractal : public Picture
{
public:

    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect::ByCenter(0.0, 0.0, 3.0);
    }

    static inline Uint32 make_color(int index)
    {
	index += 13;
	return ((index * 19) & 0xFF) << 24
	    |  ((index * 7) & 0xFF) << 16
	    |  ((index * 23) & 0xFF) << 8
	    |  0x000000FF;
    }

    int maxdepth;

    HFractal() : Picture() {
	resetVariables();
    }

    virtual void resetVariables()
    {
	maxdepth = 10;
    }

    int depth;
    double k;

    bool drawHorizontal(Canvas &c, double x, double y)
    {
	if (depth > maxdepth) return true;
	depth++;

	double x1 = x - pow(k, depth);
	double y1 = y;

	double x2 = x + pow(k, depth);
	double y2 = y;

	c.Line(x, y, x1, y1, make_color(depth));
	c.Line(x, y, x2, y2, make_color(depth));

	if (!drawVertical(c, x1, y1)) return false;
	if (!drawVertical(c, x2, y2)) return false;

	depth--;
	return true;
    }

    bool drawVertical(Canvas &c, double x, double y)
    {
	if (c.YieldUser()) return false;

	if (depth > maxdepth) return true;
	depth++;

	double x1 = x;
	double y1 = y - pow(k, depth);

	double x2 = x;
	double y2 = y + pow(k, depth);

	c.Line(x, y, x1, y1, make_color(depth));
	c.Line(x, y, x2, y2, make_color(depth));

	if (!drawHorizontal(c, x1, y1)) return false;
	if (!drawHorizontal(c, x2, y2)) return false;

	depth--;
	return true;
    }

    virtual void drawPicture(Canvas &c)
    {
	c.Blank(0x00000000);

	k = sqrt(1.0 / 2.0);
	depth = 0;

	drawHorizontal(c, 0.0, 0.0);
    }
};
