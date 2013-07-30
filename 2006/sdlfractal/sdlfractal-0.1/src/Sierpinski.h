// $Id: Sierpinski.h 9 2006-08-08 08:29:15Z tb $

#include <math.h>

class Sierpinski : public Picture
{
public:

    virtual unsigned int getOptions()
    {
	return FO_ZOOM | FO_FIXRATIO;
    }
    
    virtual Rect getDefaultViewport()
    {
	return Rect::ByCenter(0.0, 0.0, 1.0);
    }

    int maxdepth;

    Sierpinski() : Picture() {
	resetVariables();
    }

    virtual void resetVariables()
    {
	maxdepth = 5;
    }

    int depth;
    
    void drawTriangle(Canvas &canvas, double x, double y)
    {
	if (depth > maxdepth) return;
	depth++;

	double a = 1 / pow(2,depth);
	double h = a * sqrt(3);

	double Ax = x - a;
	double Ay = y - h/2;

	double Bx = x + a;
	double By = y - h/2;

	double Cx = x;
	double Cy = y + h/2;

	canvas.FilledTriangle(Ax, Ay, Bx, By, Cx, Cy, 0xFFFFFFFF);

	Ax = x - a/2;
	Ay = y;

	Bx = x + a/2;
	By = y;

	Cx = x;
	Cy = y - h/2;

	canvas.FilledTriangle(Ax, Ay, Bx, By, Cx, Cy, 0x000000FF);

	Ax = x;
	Ay = y + h/4;

	Bx = x - a/2;
	By = y - h/4;

	Cx = x + a/2;
	Cy = y - h/4;

	drawTriangle(canvas, Ax, Ay);
	drawTriangle(canvas, Bx, By);
	drawTriangle(canvas, Cx, Cy);

	depth--;
    }

    virtual void drawPicture(Canvas &c)
    {
	c.Blank(0x00000000);

	depth = 0;
	drawTriangle(c, 0.0, 0.0);
    }
};
