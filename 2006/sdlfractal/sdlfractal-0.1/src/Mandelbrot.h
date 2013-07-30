// $Id: Mandelbrot.h 13 2006-08-08 13:12:36Z tb $

class Mandelbrot : public Picture
{
public:

    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect::ByCenter(-0.5, 0.0, 3.5);
    }

    static ComplexPoint func_square(ComplexPoint p, ComplexPoint a)
    {
	return p * p + a;
    }

    static ComplexPoint func_cubic(ComplexPoint p, ComplexPoint a)
    {
	return p * p * p + a;
    }

    /// number of colors used -> maximum number of iterations
    int numcolor;

    /// calculate a rgb value from three doubles ranged [0.0 ... 1.0]
    static inline Uint32 dRGB(double r, double g, double b)
    {
	assert(0.0 <= r && r <= 1.0);
	assert(0.0 <= g && g <= 1.0);
	assert(0.0 <= b && b <= 1.0);

	return ((int(std::min(1.0, r) * 256.0) & 0xFF) << 24)
	    |  ((int(std::min(1.0, g) * 256.0) & 0xFF) << 16)
	    |  ((int(std::min(1.0, b) * 256.0) & 0xFF) << 8)
	    |  0x000000FF;
    }

    static inline Uint32 dwRGB(double r, double g, double b)
    {
	return dRGB(r - trunc(r), g - trunc(g), b - trunc(b));
    }

    /// calculate a rgb value from three integers ranges [0 .. 255]
    static inline Uint32 iRGB(int r, int g, int b)
    {
	assert(0 <= r && r <= 255);
	assert(0 <= g && g <= 255);
	assert(0 <= b && b <= 255);

	return ((r & 0xFF) << 24)
	    |  ((g & 0xFF) << 16)
	    |  ((b & 0xFF) << 8)
	    |  0x000000FF;
    }

    /// color makers structure containing name
    struct ColorMaker {
	const char*	name;
	Uint32 		(Mandelbrot::*func)(int);
    };

    /// currently selected color maker
    struct ColorMaker 	*colormaker;

    // *** Different Color Makers

    Uint32 color_graygradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(rat, rat, rat);
    }

    Uint32 color_redgradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(rat, 0, 0);
    }

    Uint32 color_greengradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(0, rat, 0);
    }

    Uint32 color_bluegradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(0, 0, rat);
    }

    Uint32 color_cyangradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(0, rat, rat);
    }

    Uint32 color_alternatinggradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dRGB(rat + 0.0, rat + 0.333, rat + 0.666);
    }

    Uint32 color_modulo1gradient(int index)
    {
	return iRGB((index * 2) % 256,
		    (index * 3) % 256,
		    (index * 5) % 256);
    }

    Uint32 color_modulo2gradient(int index)
    {
	return iRGB((index * 3) % 256,
		    (index * 7) % 256,
		    (index * 2) % 256);
    }

    Uint32 color_testgradient(int index)
    {
	double rat = double(index) / double(numcolor);
	return dwRGB(rat + 0.0, rat + 0.333, rat + 0.666);
/*
	return iRGB((index * 2) % 256,
		    (index * 3) % 256,
		    (index * 5) % 256);
*/
    }

    /// array of color makers
    struct ColorMaker colormakers[10];
    unsigned int numcolormakers;

    void setup_colormakers()
    {
	memset(colormakers, 0, sizeof(colormakers));
	int z = 0;

	colormaker[z].name = "White Gradient";
	colormaker[z++].func = &Mandelbrot::color_graygradient;

	colormaker[z].name = "Red Gradient";
	colormaker[z++].func = &Mandelbrot::color_redgradient;

	colormaker[z].name = "Green Gradient";
	colormaker[z++].func = &Mandelbrot::color_greengradient;

	colormaker[z].name = "Blue Gradient";
	colormaker[z++].func = &Mandelbrot::color_bluegradient;

	colormaker[z].name = "Cyan Gradient";
	colormaker[z++].func = &Mandelbrot::color_cyangradient;

	colormaker[z].name = "Modulo1 Gradient";
	colormaker[z++].func = &Mandelbrot::color_modulo1gradient;

	colormaker[z].name = "Modulo2 Gradient";
	colormaker[z++].func = &Mandelbrot::color_modulo2gradient;

	colormaker[z].name = "Test Gradient";
	colormaker[z++].func = &Mandelbrot::color_testgradient;

	numcolormakers = z;
    }

    /// use the current color maker to fill the color array
    void cache_colors(Uint32 *colors)
    {
	for(int z = 0; z < numcolor; z++)
	{
	    colors[z] = (this->*(colormaker->func))(z);
	}

	colors[0] = 0x000000FF;
    }

    /// constructor. set's up local color maker array
    Mandelbrot() : Picture() {
	resetVariables();
	setup_colormakers();
    }

    double max_normsquare;

    virtual void resetVariables()
    {
	numcolor = 256;
	colormaker = &colormakers[0];
	max_normsquare = 1000.0;
    }

    virtual void drawPicture(Canvas &c)
    {
	c.Blank(0x00000000);

	Uint32 colors[numcolor];

	cache_colors(colors);

	for(unsigned int px_real = 0; px_real < c.getSurfaceWidth(); px_real++)
	{
	    for(unsigned int px_imag = 0; px_imag < c.getSurfaceHeight(); px_imag++)
	    {
		const ComplexPoint a (c.screen2worldX(px_real), c.screen2worldY(px_imag));
		ComplexPoint p = a;
            
		int n;
		for(n = 0; n < numcolor; n++)
		{
		    p = func_square(p, a);

		    double norm = p.SquareNorm();

		    if (norm > max_normsquare) break;
		}
            
		if (n >= numcolor) n = 0;

		c.fastPixelNolock(px_real, px_imag, colors[n]);

		if (c.YieldUser()) return;
	    }
	}
    }
};

