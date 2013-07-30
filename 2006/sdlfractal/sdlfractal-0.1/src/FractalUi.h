// $Id: FractalUi.h 9 2006-08-08 08:29:15Z tb $
// included from fractalwin.cxx to add callbacks

#include <stdio.h>

#include <FL/Fl_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_File_Chooser.H>

#include "Surface.h"
#include "Canvas.h"

#include "Picture.h"
#include "Mandelbrot.h"
#include "Feigenbaum.h"
#include "Mira.h"
#include "HFractal.h"
#include "PythagorasTree.h"
#include "Sierpinski.h"

/// Defines the callbacks for the controller window, holds the screen canvas
/// and encloses instances of all fractals.
class FractalUi
{
public:
    /// SDL drawing canvas
    Canvas screen;

    /// currently displayed fractal
    Picture* currpic;

    /// ourselves
    class FractalWin *thiswin;

    /// initialize the canvas and fractals
    FractalUi()
    {
	thiswin = NULL;

	screen.Init(800, 600);
	SDL_WM_SetCaption("Fractal Canvas", NULL);

	// set default picture
	currpic = &mandelbrot;
	screen.alwaysfixratio = currpic->getOptions() & Picture::FO_FIXRATIO;
	screen.setViewportBounding(currpic->getDefaultViewport());
	drawPicture();
    }

    /// the main window closes
    void WinClose(Fl_Window *win)
    {
	SDL_Quit();
	win->hide();
	exit(0);
    }

    /// disable and enable the ui
    void deactivateUi();
    void activateUi();

    /// draw the currently selected fractal
    void drawPicture()
    {
	deactivateUi();

	if (!currpic) {
	    screen.Blank(0x00000000);
	    screen.Flip();

	    activateUi();
	    return;
	}

	screen.Lock();
	currpic->drawPicture(screen);
	screen.Unlock();
	screen.Flip();

	activateUi();
    }

    /// *** Mandelbrot Fractal

    Mandelbrot	mandelbrot;

    void Mandelbrot_Choice_ColorScheme(Fl_Choice *choice)
    {
	assert(choice->value() < (int)mandelbrot.numcolormakers);

	mandelbrot.colormaker = &mandelbrot.colormakers[choice->value()];
	drawPicture();
    }

    void Mandelbrot_Set_MaxSquareNorm(Fl_Value_Input *vi)
    {
	mandelbrot.max_normsquare = vi->value();
	drawPicture();
    }

    void Mandelbrot_Update_numcolor(Fl_Value_Input *vi)
    {
	mandelbrot.numcolor = int(vi->value());
	drawPicture();
    }

    /// *** Feigenbaum

    Feigenbaum	feigenbaum;

    /// *** Mira Fractal

    Mira	mira;

    void Update_Mira_A(Fl_Value_Input *vi)
    {
	mira.a = vi->value();
	drawPicture();
    }

    void Update_Mira_B(Fl_Value_Input *vi)
    {
	mira.b = vi->value();
	drawPicture();
    }

    void Update_Mira_Iterations(Fl_Value_Input *vi)
    {
	mira.iterations = int(vi->value());
	drawPicture();
    }

    void Update_Mira_DrawOffset(Fl_Value_Input *vi)
    {
	mira.drawoffset = int(vi->value());
	drawPicture();
    }

    /// *** H-Fractal
    
    HFractal	hfractal;

    void Update_HFractal_Order(Fl_Value_Input *vi)
    {
	hfractal.maxdepth = int(vi->value());
	drawPicture();
    }

    /// *** PythagorasTree
    
    PythagorasTree pythagorastree;

    void Update_PythagorasTree_Order(Fl_Value_Input *vi)
    {
	pythagorastree.maxdepth = int(vi->value());
	drawPicture();
    }

    /// *** Sierpinski Triangle

    Sierpinski	sierpinski;

    void Update_Sierpinski_Order(Fl_Value_Input *vi)
    {
	sierpinski.maxdepth = int(vi->value());
	drawPicture();
    }

    /// *** Other Functions

    /// changing tabs selects a different fractal type
    void TabChange(Fl_Tabs* tabs, long tabset)
    {
	Fl_Widget *vistab = tabs->value();

	switch((int)vistab->user_data() + tabset * 10)
	{
	case 1:
	    currpic = &mandelbrot;
	    break;

	case 2:
	    currpic = &feigenbaum;
	    break;

	case 3:
	    currpic = &mira;
	    break;

	case 14:
	    currpic = &hfractal;
	    break;

	case 15:
	    currpic = &pythagorastree;
	    break;

	case 16:
	    currpic = &sierpinski;
	    break;

	default:
	    printf("Unknown tab number: %d\n", (int)vistab->user_data());
	    break;
	}

	// update the view port and draw the new picture
	screen.alwaysfixratio = currpic->getOptions() & Picture::FO_FIXRATIO;
	screen.setViewportBounding(currpic->getDefaultViewport());

	drawPicture();
    }

    /// save the image at a different resolution to a png file
    void Button_SavePNG(double width)
    {
	if (!currpic) return;

	double height = width * screen.getSurfaceHeight() / screen.getSurfaceWidth();

	printf("PNG Resolution: %f x %f\n", width, height);

	Fl_File_Chooser *fchoose = new Fl_File_Chooser(NULL, "PNG Files (*.png)", Fl_File_Chooser::CREATE, "Select PNG Filename");
	fchoose->show();
	while(fchoose->shown())
	    Fl::wait();

	if (!fchoose->value()) return;

	deactivateUi();

	Canvas surface;
	surface.InitSW(int(width), int(height));

	surface.alwaysfixratio = currpic->getOptions() & Picture::FO_FIXRATIO;
	surface.setViewportBounding(screen.getViewport());

	surface.Lock();
	currpic->drawPicture(surface);
	surface.Unlock();

	surface.WritePNG(fchoose->value());

	activateUi();
    }

    /// redraw current picture
    void Button_Redraw()
    {
	drawPicture();
    }
};
