// $Id: Feigenbaum.h 9 2006-08-08 08:29:15Z tb $

class Feigenbaum : public Picture
{
public:
    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect(2.9, 0.0, 4.0, 1.0);
    }


    Feigenbaum() : Picture()
    {
	resetVariables();
    }

    virtual void resetVariables()
    {
    }
    
    double f(double x, double a)
    {
	return a * x * (1.0 - x);
    }

    virtual void drawPicture(Canvas &c)
    {
	c.Blank(0x00000000);

	for(double a = c.getViewportLeft(); a < c.getViewportRight(); a += c.getPixelWidth())
	{
	    double x = 0.1;

	    for(int z = 0; z < 300; z++)
	    {
		x = f(x,a);
	    }

	    for(int z = 0; z < 200; z++)
	    {
		c.Dot(a,x);

		x = f(x,a);
	    }
	}
    }
};
