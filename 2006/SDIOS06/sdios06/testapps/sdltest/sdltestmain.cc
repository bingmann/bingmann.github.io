#include <config.h>

#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include <SDL_image.h>

#ifdef SDIOS
#define LOGO "/minixfs0/test.png"
#else
#define LOGO "test.png"
#endif

void draw_colormap(SDL_Surface* screen)
{
	float redfrom = 0.835294117647059;
	float greenfrom = 0.0823529411764706;
	float bluefrom = 0.505882352941176;
	float redto = 1.0;
	float greento = 1.0;
	float blueto = 0;
	
	float bluespeed = (blueto - bluefrom) / screen->h;
	float greenspeed = (greento - greenfrom) / screen->h;
	float redspeed = (redto - redfrom) / screen->h;
	
	SDL_Rect rect;
	rect.x = 0;
	rect.w = screen->w;
	rect.h = 1;
	for(int y = 0; y < screen->h; ++y) {
		rect.y = y;
		
		uint8_t r = static_cast<uint8_t> ((redfrom + (redspeed * y)) * 255.0);
		uint8_t g = static_cast<uint8_t> ((greenfrom + (greenspeed * y)) * 255.0);
		uint8_t b = static_cast<uint8_t> ((bluefrom + (bluespeed * y)) * 255.0);
		
		uint32_t col = SDL_MapRGB(screen->format, r, g, b);
		SDL_FillRect(screen, &rect, col);
	}
}

int main()
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) != 0) {
		printf("SDL_INIT failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 0, 0);
	if(!screen) {
		printf("SetVideoMode failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface* image = IMG_Load(LOGO);
	if(image == NULL) {
		printf("Couldn't load image: %s\n", SDL_GetError());
	} else {
		printf("Image loaded\n");
	}

	SDL_Surface* backbuffer = SDL_CreateRGBSurface(0,
			screen->w, screen->h, screen->format->BitsPerPixel,
			screen->format->Rmask, screen->format->Gmask,
			screen->format->Bmask, screen->format->Amask);

	SDL_Surface* dest = NULL;
	if(image != NULL) {
		dest = SDL_CreateRGBSurface(0, image->w, image->h,
		  		image->format->BitsPerPixel,
	  			image->format->Rmask, image->format->Gmask,
  				image->format->Bmask, image->format->Amask);
		if(dest == NULL) {
			printf("Out of memory\n");
			return 1;
		}
		SDL_SetAlpha(image, 0, 0);
		SDL_SetAlpha(dest, SDL_SRCALPHA, 0);
	}

	Uint32 startticks = SDL_GetTicks();
	Uint32 ticks = startticks;
	const Uint32 fadetime = 1024;
	float frames = 0;
	bool running = true;
	while(running) {
		Uint32 tcks = ticks - startticks;
		if(image != NULL) {
			SDL_BlitSurface(image, NULL, dest, NULL);
			if(tcks > fadetime)
				tcks = fadetime;
			int alphaoffset = dest->format->Ashift / 8;
			
			// adjust alpha values...
			uint8_t* pix = (uint8_t*) dest->pixels;
			for(int y = 0; y < dest->h; ++y) {
				uint8_t* a = pix + alphaoffset;
				for(int x = 0; x < dest->w; ++x) {
					*a = (uint8_t) ((((uint32_t) (*a)) * tcks) / fadetime);
					a += 4;
				}
				pix += dest->pitch;
			}
		}
		
		draw_colormap(backbuffer);
		
		if(image != NULL) {
			SDL_Rect drect;
			drect.x = (screen->w - image->w) / 2;
			drect.y = (screen->h - image->h) / 2;
			SDL_BlitSurface(dest, NULL, backbuffer, &drect);
		}
		
		SDL_BlitSurface(backbuffer, NULL, screen, NULL);
		SDL_Flip(screen);
		frames++;

		ticks = SDL_GetTicks();
#if 0
		if(ticks - startticks > 5000)
			break;
#endif

		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					printf("KeyDown (sym %d - %s)\n", event.key.keysym.sym,
					       SDL_GetKeyName(event.key.keysym.sym));
					if(event.key.keysym.sym == SDLK_ESCAPE)
						running = false;
					break;
				case SDL_KEYUP:
					printf("KeyUp (sym %d - %s)\n", event.key.keysym.sym,
					       SDL_GetKeyName(event.key.keysym.sym));
					break;
			}
		}
	}
	printf("FPS: %f\n", frames / 5.0);

	if(image != NULL)
		SDL_FreeSurface(image);
	if(backbuffer != NULL)
		SDL_FreeSurface(backbuffer);

	SDL_Quit();
	printf("everything ok, quitting\n");
	return 0;
}

