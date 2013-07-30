// $Id: Mira.h 9 2006-08-08 08:29:15Z tb $

#include <math.h>

class Mira : public Picture
{
public:

    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect::ByCenter(0.0, 0.0, 80.0);
    }

    static inline Uint32 make_color(int index)
    {
	return ((index * 3) & 0xFF) << 24
	    |  ((index * 2) & 0xFF) << 16
	    |  ((index * 7) & 0xFF) << 8
	    |  0x000000FF;
    }

    /// parameters to the fractal
    double a, b;

    /// number of iterations (points to draw)
    int	iterations;

    /// skip n points at the beginning
    int drawoffset;
    
    Mira() : Picture() {
	resetVariables();
    }

    virtual void resetVariables()
    {
	a = 0.7;
	b = 0.998;
	iterations = 1000000;
	drawoffset = 100;
    }

    double F(float x)
    {
	return a*x + 2 * x*x * (1 - a) / (1 + x*x);
    }

    virtual void drawPicture(Canvas &canvas)
    {
	canvas.Blank(0x00000000);

	double x = 0;
	double y = -20.1;

	double w = F(x);

	for(int z=0;z < iterations;z++)
	{
	    if (z > drawoffset)
	    {
		canvas.Dot(x, y, make_color(z / 100));
	    }

	    float _x = x;
        
	    x = y + w;
	    w = F(x);
	    y = -b*_x + w;

	    if (canvas.YieldUser()) return;
	}

	printf("a = %.4f; b = %.5f\n",a,b);
    }
};
