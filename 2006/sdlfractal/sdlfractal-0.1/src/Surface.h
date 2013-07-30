// $Id: Surface.h 9 2006-08-08 08:29:15Z tb $
// Surface Object supporting drawing primitives.

#include "SDL_gfxPrimitives.h"

#include <algorithm>
#include <png.h>

class Surface
{
private:
    /// enclosed SDL surface
    SDL_Surface*	surface;

public:

    /// create a new surface with given size
    inline bool Init(int w, int h)
    {
	// | SDL_ANYFORMAT
	surface = SDL_SetVideoMode(w, h, 24, SDL_HWSURFACE | SDL_DOUBLEBUF  | SDL_HWPALETTE | SDL_RESIZABLE);
	if ( surface == NULL ) {
	    fprintf(stderr, "Couldn't set %dx%dx24 video mode: %s\n", w, h, SDL_GetError());
	    return false;
	}

	fprintf(stderr, "Got SDL surface: %dx%dx%d\n", surface->w, surface->h, surface->format->BitsPerPixel);
	return true;
    }

    /// create a new software surface with given size
    inline bool InitSW(int w, int h)
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
				       rmask, gmask, bmask, amask);

	if ( surface == NULL ) {
	    fprintf(stderr, "Couldn't set %dx%dx24 video mode: %s\n", w, h, SDL_GetError());
	    return false;
	}

	fprintf(stderr, "Got SDL software surface: %dx%dx%d - %d\n", surface->w, surface->h, surface->format->BitsPerPixel, surface->format->BytesPerPixel);
	return true;
    }

    /// free the surface
    inline void Free()
    {
	SDL_FreeSurface(surface);
	surface = NULL;
    }

    /// fetch the surface's width
    inline unsigned int	getSurfaceWidth() const
    {
	return surface->w;
    }

    /// fetch the surface's height
    inline unsigned int	getSurfaceHeight() const
    {
	return surface->h;
    }

    /// lock the surface before drawing
    inline void Lock()
    {
	if ( SDL_MUSTLOCK(surface) ) {
	    if ( SDL_LockSurface(surface) < 0 ) {
		fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
	    }
	}
    }

    /// unlock the surface after drawing
    inline void Unlock()
    {
	if ( SDL_MUSTLOCK(surface) ) {
	    SDL_UnlockSurface(surface);
	}
    }

    /// flip the changes to the screen
    inline void Flip()
    {
	SDL_Flip(surface);
    }

    /// map a color to the surface's pixel format
    inline Uint32 MapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
    {
	return SDL_MapRGBA(surface->format, r, g, b, a);
    }

    /// map a color to the surface's pixel format
    inline Uint32 MapRGBA(Uint32 rgba) const
    {
	return SDL_MapRGBA(surface->format,
			   (rgba & 0xFF000000) >> 24,
			   (rgba & 0x00FF0000) >> 16,
			   (rgba & 0x0000FF00) >> 8,
			   (rgba & 0x000000FF) >> 0);
    }

    /// blank out the whole surface with this color
    inline void Blank(Uint32 color)
    {
	SDL_FillRect(surface, NULL, MapRGBA(color));
    }

    /// putpixel to the surface
    inline void PutPixel(int x, int y, Uint32 color)
    {
	// call SDL_gfx
	pixelColor(surface, x, y, color);
    }
    
    /// fast and dangerous putpixel with no locking
    inline void fastPixelNolock(Sint16 x, Sint16 y, Uint32 color)
    {
	int bpp;
	Uint8 *p;

	color = MapRGBA(color);

	if ((x >= surface->clip_rect.x) && (x <= surface->clip_rect.x+surface->clip_rect.w-1) &&
	    (y >= surface->clip_rect.y) && (y <= surface->clip_rect.y+surface->clip_rect.h-1))
	{
	    bpp = surface->format->BytesPerPixel;
	    p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;
	    switch (bpp) {
	    case 1:
		*p = color;
		break;
	    case 2:
		*(Uint16 *) p = color;
		break;
	    case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		    p[0] = (color >> 16) & 0xff;
		    p[1] = (color >> 8) & 0xff;
		    p[2] = color & 0xff;
		} else {
		    p[0] = color & 0xff;
		    p[1] = (color >> 8) & 0xff;
		    p[2] = (color >> 16) & 0xff;
		}
		break;
	    case 4:
		*(Uint32 *) p = color;
		break;
	    }
	}
    }

    /// draw a line from (x1,y1) to (x2,y2)
    inline void DrawLine(int x1, int y1, int x2, int y2, Uint32 color)
    {
	// from SDL_gfx
	lineColor(surface, x1, y1, x2, y2, color);
    }

    /// draw an anti-aliased line from (x1,y1) to (x2,y2)
    inline void DrawLineAA(int x1, int y1, int x2, int y2, Uint32 color)
    {
	// from SDL_gfx
	aalineColor(surface, x1, y1, x2, y2, color);
    }

    inline void DrawFilledTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Uint32 color)
    {
	// from SDL_gfx
	filledTrigonColor(surface, x1, y1, x2, y2, x3, y3, color);
    }

    /// xor a color to a pixel, modified from SDL_gfx
    inline int xorFastPixelColorNolockNoclip(Sint16 x, Sint16 y, Uint32 color)
    {
	int bpp;
	Uint8 *p;

	/*
	 * Get destination format 
	 */
	bpp = surface->format->BytesPerPixel;
	p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;
	switch (bpp) {
	case 1:
	    *p ^= color;
	    break;
	case 2:
	    *(Uint16 *) p ^= color;
	    break;
	case 3:
	    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		p[0] ^= (color >> 16) & 0xff;
		p[1] ^= (color >> 8) & 0xff;
		p[2] ^= color & 0xff;
	    } else {
		p[0] ^= color & 0xff;
		p[1] ^= (color >> 8) & 0xff;
		p[2] ^= (color >> 16) & 0xff;
	    }
	    break;
	case 4:
	    *(Uint32 *) p ^= color;
	    break;
	} /* switch */

	return (0);
    }

    /// draw an xor rectangle
    inline void DrawXORRectangle(int x1, int y1, int x2, int y2, Uint32 color)
    {
	if (x1 > x2) std::swap(x1,x2);
	if (y1 > y2) std::swap(y1,y2);

	Lock();

	for(int x = x1; x <= x2; x++)
	{
	    xorFastPixelColorNolockNoclip(x, y1, color);
	    if (y1 != y2) xorFastPixelColorNolockNoclip(x, y2, color);
	}
	for(int y = y1+1; y < y2; y++)
	{
	    xorFastPixelColorNolockNoclip(x1, y, color);
	    if (x1 != x2) xorFastPixelColorNolockNoclip(x2, y, color);
	}

	Unlock();
    }

    /// write the surface to a png file
    char WritePNG(const char *file)
    {
	// Open PNG file for writting
	FILE *fp = fopen(file, "wb");
	if (!fp) return 4;

	// Allocate basic libpng structures
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) { fclose(fp); return 1; }

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
	    png_destroy_write_struct(&png_ptr, 0);
	    fclose(fp); return 1;
	}

	// setjmp() must be called in every function
	// that calls a PNG-reading libpng function
	if (setjmp(png_ptr->jmpbuf))
	{
	    png_destroy_write_struct(&png_ptr, &info_ptr);
	    fclose(fp); return 3;
	}

	png_init_io(png_ptr, fp);
	// set the zlib compression level

	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
	// write PNG info to structure

	png_set_IHDR(png_ptr, info_ptr,
		     surface->w, surface->h, 8,
		     PNG_COLOR_TYPE_RGB_ALPHA,
		     PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE,
		     PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	// setjmp() must be called in every function
	// that calls a PNG-writing libpng function
	if (setjmp(png_ptr->jmpbuf))
	{
	    png_destroy_write_struct(&png_ptr, &info_ptr);
	    fclose(fp); return 3;
	}

	// Allocate pointers for each line
	png_bytepp row_pointers = static_cast<png_bytepp>(malloc(surface->h * sizeof(png_bytep)));
	if (!row_pointers)
	{
	    png_destroy_write_struct(&png_ptr, &info_ptr);
	    fclose(fp); return 1;
	}

	// how many bytes in each row
	png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	assert(rowbytes == surface->pitch);

	Lock();

	// set the individual row_pointers to point at the correct offsets
	for (int i = 0; i < surface->h; ++i)
	    row_pointers[i] = (unsigned char*)surface->pixels + i * rowbytes;

	// now we can go ahead and just write the whole image
	png_write_image(png_ptr, row_pointers);

	Unlock();

	png_write_end(png_ptr, 0);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	free(row_pointers);
	fclose(fp);

	return 0;
    }
};
