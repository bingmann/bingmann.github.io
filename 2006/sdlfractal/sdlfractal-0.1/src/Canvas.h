// $Id: Canvas.h 12 2006-08-08 10:09:35Z tb $
// Canvas does the coordinate transformation

#include <algorithm>

/// a point in world coordinates
struct Point
{
    double	x, y;

    inline Point()
    { }

    inline Point(double _x, double _y)
	: x(_x), y(_y)
    { }

    inline void clear()
    { x = y = 0.0; }

    inline double distance(const Point &o) const
    {
	return sqrt((x - o.x) * (x - o.x) + (y - o.y) * (y - o.y));
    }
};

struct ComplexPoint
{
    double	re, im;

    inline ComplexPoint()
    { }

    inline ComplexPoint(double real, double imag)
	: re(real), im(imag)
    { }

    inline double SquareNorm()
    {
	return re * re + im * im;
    }

    inline ComplexPoint& operator+=(const ComplexPoint &p)
    {
	re += p.re;
	im += p.im;
	return *this;
    }

    inline ComplexPoint& operator*=(const ComplexPoint &p)
    {
	double nre = re * p.re - im * p.im;
	im = re * p.im + im * p.re;
	re = nre;
	return *this;
    }
};

static inline ComplexPoint operator+(const ComplexPoint &a, const ComplexPoint &b)
{
    return ComplexPoint( a.re + b.re, a.im + b.im );
}

static inline ComplexPoint operator*(const ComplexPoint &a, const ComplexPoint &b)
{
    return ComplexPoint( a.re * b.re - a.im * b.im,
			 a.re * b.im + a.im * b.re );
}

struct Rect
{
    double 	x1, y1, x2, y2;

    inline Rect(double _x1, double _y1, double _x2, double _y2)
	: x1(_x1), y1(_y1), x2(_x2), y2(_y2)
    { }

    inline Rect(Point a, Point b)
	: x1(std::min(a.x, b.x)),
	  y1(std::min(a.y, b.y)),
	  x2(std::max(a.x, b.x)),
	  y2(std::max(a.y, b.y))
    { }

    static inline Rect ByCenter(double x, double y, double r)
    {
	return Rect(x - r / 2.0,
			 y - r / 2.0,
			 x + r / 2.0,
			 y + r / 2.0);
    }

    inline Point getUpperLeftPoint() const
    {
	return Point( std::min(x1, x2), std::min(y1, y2) );
    }
    inline Point getUpperRightPoint() const
    {
	return Point( std::max(x1, x2), std::min(y1, y2) );
    }
    inline Point getLowerLeftPoint() const
    {
	return Point( std::min(x1, x2), std::max(y1, y2) );
    }
    inline Point getLowerRightPoint() const
    {
	return Point( std::max(x1, x2), std::max(y1, y2) );
    }
};

class Canvas : public Surface
{
private:

    /// world view port bounding box's points
    Point		view1, view2;

    /// whether to flip or mirror the view port
    bool		flip, mirror;

    /// cached coordinate transformation scale parameters
    double		scaleX, scaleY;

public:

    /// whether to correct the view port ratio
    bool		alwaysfixratio;

    /// update transformation parameters
    inline void		updateTransformation()
    {
	if (alwaysfixratio) fixViewportRatio();

	scaleX = getSurfaceWidth() / getViewportWidth();
	scaleY = getSurfaceHeight() / getViewportHeight();

	scaleX *= (mirror ? -1 : +1);
	scaleY *= (flip ? -1 : +1);
    }

    /// constructor
    inline Canvas()
	: Surface()
    {
	clear();
    }

    /// reset the surface
    inline void		clear()
    {
	view1.clear();
	view2.clear();

	flip = mirror = false;
	scaleX = scaleY = 0.0;
    }

    /// return the view port rectangle
    inline Rect	getViewport() const
    {
	return Rect(view1, view2);
    }

    /// calculate the center of the view port
    inline Point	getViewportCenter() const
    {
	return Point((view1.x + view2.x) / 2.0,
		     (view1.y + view2.y) / 2.0);
    }

    /// width of the view port
    inline double	getViewportWidth() const
    {
	return view2.x - view1.x;
    }

    /// height of the view port
    inline double	getViewportHeight() const
    {
	return view2.y - view1.y;
    }

    /// return the left edge of the view port
    inline double	getViewportLeft() const
    {
	return view1.x;
    }

    /// return the right edge of the view port
    inline double	getViewportRight() const
    {
	return view2.x;
    }

    /// return the bottom edge of the view port
    inline double	getViewportBottom() const
    {
	return view1.y;
    }

    /// return the top edge of the view port
    inline double	getViewportTop() const
    {
	return view2.y;
    }

    /// change the view port by specifying the two bounding points
    inline void		setViewportBounding(Point p1, Point p2)
    {
	view1.x = std::min(p1.x, p2.x);
	view1.y = std::min(p1.y, p2.y);

	view2.x = std::max(p1.x, p2.x);
	view2.y = std::max(p1.y, p2.y);

	updateTransformation();
    }

    /// change the view port by specifying the bounding rectangle
    inline void		setViewportBounding(Rect r)
    {
	setViewportBounding(r.getUpperLeftPoint(), r.getLowerRightPoint());
    }

    /// change the view port by specifying the new center point and the view's width
    inline void		setViewportCenter(Point c, double w)
    {
	double h = w * static_cast<double>(getSurfaceHeight()) / static_cast<double>(getSurfaceWidth());

 	view1.x = c.x - w / 2;
	view1.y = c.y - h / 2;

 	view2.x = c.x + w / 2;
	view2.y = c.y + h / 2;

	updateTransformation();
    }

    /// set the flip parameter
    inline void		toggleViewportFlip()
    {
	flip ^= 1;
	updateTransformation();
    }

    /// set the mirror parameter
    inline void		toggleViewportMirror()
    {
	mirror ^= 1;
	updateTransformation();
    }

    /// transform a point from world coords to surface coords
    inline int		world2screenX(double x)
    {
	return static_cast<int>((x - view1.x) * scaleX);
    }

    /// transform a point from world coords to surface coords
    inline int		world2screenY(double y)
    {
	return static_cast<int>((view2.y - y) * scaleY);
    }

    /// transform a point from the screen to world coords
    inline double	screen2worldX(int x)
    {
	return ((double)x / scaleX) + view1.x;
    }

    /// transform a point from the screen to world coords
    inline double	screen2worldY(int y)
    {
	return -((double)y / scaleY) + view2.y;
    }

    /// return the width of a single pixel on the screen.
    inline double	getPixelWidth() const
    {
	return getViewportWidth() / getSurfaceWidth();
    }

    /// return the height of a single pixel on the screen.
    inline double	getPixelHeight() const
    {
	return getViewportHeight() / getSurfaceHeight();
    }

    /// fix view port ratio
    inline void		fixViewportRatio()
    {
	double surfaceRatio = (double)getSurfaceWidth() / (double)getSurfaceHeight();
	double viewportRatio = getViewportWidth() / getViewportHeight();

	//printf("surface: %f viewport: %f\n", surfaceRatio, viewportRatio);
	
	if (surfaceRatio < viewportRatio)
	{
	    Point c = getViewportCenter();
	    double r = static_cast<double>(getSurfaceHeight()) / static_cast<double>(getSurfaceWidth());
	    double w = getViewportWidth();
	    double h = w * r;

	    view1.x = c.x - w / 2;
	    view1.y = c.y - h / 2;

	    view2.x = c.x + w / 2;
	    view2.y = c.y + h / 2;
	}
	else
	{
	    Point c = getViewportCenter();
	    double r = static_cast<double>(getSurfaceHeight()) / static_cast<double>(getSurfaceWidth());
	    double h = getViewportHeight();
	    double w = h / r;

	    view1.x = c.x - w / 2;
	    view1.y = c.y - h / 2;

	    view2.x = c.x + w / 2;
	    view2.y = c.y + h / 2;
	}
    }

    // *** Drawing Primitives

    /// yield and check the user pressed a key.
    inline bool		YieldUser()
    {
	SDL_Event event;

	Fl::check();

	if ( SDL_PollEvent(&event) )
	{
	    switch (event.type)
	    {
	    case SDL_MOUSEBUTTONDOWN:
	    case SDL_VIDEORESIZE:
	    case SDL_KEYDOWN:
	    case SDL_QUIT:
	    {
		// put event back into queue
		SDL_PushEvent(&event);

		// and tell the drawing method to stop
		return true;
	    }
	    }
	}

	return false;
    }

    /// put a pixel on the screen
    inline void		Dot(double x, double y, Uint32 color = 0xFFFFFFFF)
    {
	PutPixel( world2screenX(x), world2screenY(y), color );
    }

    /// put a pixel on the screen - no locking or clipping
    inline void		fastDot(double x, double y, Uint32 color = 0xFFFFFFFF)
    {
	fastPixelNolock( world2screenX(x), world2screenY(y), color );
    }


    /// draw a line on the screen
    inline void		Line(double x1, double y1, double x2, double y2, Uint32 color = 0xFFFFFFFF)
    {
	DrawLine( world2screenX(x1), world2screenY(y1),
		  world2screenX(x2), world2screenY(y2),
		  color );
    }

    /// draw a line on the screen
    inline void		Line(Point a, Point b, Uint32 color = 0xFFFFFFFF)
    {
	Line(a.x, a.y, b.x, b.y, color);
    }

    /// draw filled triangle
    inline void		FilledTriangle(double x1, double y1,
				       double x2, double y2,
				       double x3, double y3,
				       Uint32 color = 0xFFFFFFFF)
    {
	DrawFilledTriangle( world2screenX(x1), world2screenY(y1),
			    world2screenX(x2), world2screenY(y2),
			    world2screenX(x3), world2screenY(y3),
			    color );
    }
};
