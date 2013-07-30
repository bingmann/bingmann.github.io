# $Id: Makefile.am 16 2006-08-09 19:11:31Z tb $

bin_PROGRAMS = sdlfractal

sdlfractal_SOURCES = \
	main.cc SDL_gfxPrimitives.c \
	FractalWin.cc FractalWin.h FractalWin.fl \
	Canvas.h Surface.h \
	SDL_gfxPrimitives.h SDL_gfxPrimitives_font.h \
	Picture.h FractalUi.h \
	Mandelbrot.h Mira.h Sierpinski.h \
	HFractal.h PythagorasTree.h Feigenbaum.h \
	sdlfractal.rc sdlfractal.ico

AM_CFLAGS = -W -Wall -O3
AM_CXXFLAGS = -W -Wall -O3
LDADD =

FractalWin.cc: FractalWin.fl
	fluid -c FractalWin.fl

FractalWin.h: FractalWin.fl
	fluid -c FractalWin.fl

if GOT_RESCOMP
sdlfractal-rc.o: sdlfractal.rc
	$(RESCOMP) -I$(srcdir) $(srcdir)/sdlfractal.rc sdlfractal-rc.o

LDADD += sdlfractal-rc.o
endif
