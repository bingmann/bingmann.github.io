// $Id: main.cc 9 2006-08-08 08:29:15Z tb $

#include <SDL.h>
#include <stdlib.h>

#include "FractalWin.h"

void FractalWin::UpdateValues()
{
    thiswin = this;

    // set all fractals' variables
    for(unsigned int i = 0; mandelbrot.colormakers[i].func; i++)
    {
	cb_mandelbrot_colorscheme->add(mandelbrot.colormakers[i].name);
    }
    cb_mandelbrot_colorscheme->value(0);

    vi_mandelbrot_numcolor->value(mandelbrot.numcolor);
    vi_mandelbrot_maxsquarenorm->value(mandelbrot.max_normsquare);
    
    vi_mira_a->value(mira.a);
    vi_mira_b->value(mira.b);
    vi_mira_iterations->value(mira.iterations);
    vi_mira_drawoffset->value(mira.drawoffset);

    vi_hfractal_order->value(hfractal.maxdepth);
    vi_pythagorastree_order->value(pythagorastree.maxdepth);
    vi_sierpinski_order->value(sierpinski.maxdepth);

    vi_pngwidth->value(800);
}

void FractalUi::activateUi()
{
    if (!thiswin) return;

    thiswin->WMain->activate();
}

void FractalUi::deactivateUi()
{
    if (!thiswin) return;

    thiswin->WMain->deactivate();
}

struct
{
    // 0 = no dragging, 1 = drag pending after a few pixel, 2 = dragging
    int		status;

    // start and end points of the drag
    unsigned int startx, starty;
    unsigned int endx, endy;
    
} drag;

int main(int argc, char *argv[])
{
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
	return 1;
    }

    atexit(SDL_Quit);

    // construct the user interface window

    FractalWin ui;
    ui.WMain->show(argc, argv);

    // sdl and fltk event loop

    SDL_Event event;

    while(1)
    {
	Fl::check();

	while ( SDL_PollEvent(&event) )
	{
	    switch (event.type)
	    {
	    case SDL_MOUSEBUTTONDOWN:
	    {
		if (event.button.button == SDL_BUTTON_LEFT)
		{
		    drag.status = 1;
		    drag.startx = event.button.x;
		    drag.starty = event.button.y;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
		    ui.screen.setViewportCenter( ui.screen.getViewportCenter(),
						 ui.screen.getViewportWidth() * 2.5 );

		    ui.drawPicture();
		}

		break;
	    }
	    case SDL_MOUSEMOTION:
	    {
		if (drag.status == 1)
		{
		    if (std::abs((long)drag.startx - event.motion.x) + std::abs((long)drag.starty - event.motion.y) > 4)
		    {
			drag.status = 2;
			drag.endx = event.motion.x;
			drag.endy = event.motion.y;

			ui.screen.DrawXORRectangle(drag.startx, drag.starty,
						   event.motion.x, event.motion.y,
						   ui.screen.MapRGBA(0xFFFFFF00));
			ui.screen.Flip();
		    }
		}
		else if (drag.status == 2)
		{
		    ui.screen.DrawXORRectangle(drag.startx, drag.starty,
					       drag.endx, drag.endy,
					       ui.screen.MapRGBA(0xFFFFFF00));

		    drag.endx = event.motion.x;
		    drag.endy = event.motion.y;

		    ui.screen.DrawXORRectangle(drag.startx, drag.starty,
					       drag.endx, drag.endy,
					       ui.screen.MapRGBA(0xFFFFFF00));
		    ui.screen.Flip();
		}

#if 0
		printf("Mouse: (%d,%d) -> (%f,%f)\n",
		       event.motion.x, event.motion.y,
		       ui.screen.screen2worldX(event.motion.x),
		       ui.screen.screen2worldY(event.motion.y));
#endif
		break;
	    }
	    case SDL_MOUSEBUTTONUP:
	    {
		if (event.button.button == SDL_BUTTON_LEFT)
		{
		    printf("Mouse button released at (%d,%d)\n", event.button.x, event.button.y);

		    if (drag.status == 2)
		    {
			ui.screen.DrawXORRectangle(drag.startx, drag.starty,
						   drag.endx, drag.endy,
						   ui.screen.MapRGBA(0xFFFFFF00));
			ui.screen.Flip();

			ui.screen.setViewportBounding(Rect(ui.screen.screen2worldX(drag.startx),
							   ui.screen.screen2worldY(drag.starty),
							   ui.screen.screen2worldX(drag.endx),
							   ui.screen.screen2worldY(drag.endy)));
			ui.drawPicture();
		    }

		    drag.status = 0;
		}

		break;
	    }
	    case SDL_VIDEORESIZE:
	    {
		printf("SDL screen resized to %dx%d\n", event.resize.w, event.resize.h);
		
		ui.vi_pngwidth->value(event.resize.w);

		ui.screen.Init(event.resize.w, event.resize.h);
		ui.screen.updateTransformation();
		ui.drawPicture();

		break;
	    }
	    case SDL_QUIT:
		exit(0);
		break;
	    }
	}

	SDL_Delay(100);
    }

    /* This should never happen */
    printf("SDL_WaitEvent error: %s\n", SDL_GetError());
    return 1;
}
