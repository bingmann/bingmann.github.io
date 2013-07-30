/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include <config.h>
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include <assert.h>

#include "SDL_sdivideo.h"
#include "SDL_sdievents_c.h"
#include "SDL_sdimouse_c.h"

#include <l4/thread.h>
#include <l4/sigma0.h>
#include <l4/kdebug.h>
#include <l4/schedule.h>
#include <l4io.h>

#include <sdi/locator.h>
#include <sdi/log.h>

#include <idl4/idl4.h>

#include <idl4glue.h>
#include <if/ifframebuffer.h>

#define SDIVID_DRIVER_NAME "sdi"

/* Initialization/Query functions */
static int SDI_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **SDI_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *SDI_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int SDI_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void SDI_VideoQuit(_THIS);

/* Hardware surface functions */
static int SDI_AllocHWSurface(_THIS, SDL_Surface *surface);
static int SDI_LockHWSurface(_THIS, SDL_Surface *surface);
static void SDI_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void SDI_FreeHWSurface(_THIS, SDL_Surface *surface);
static int SDI_FlipHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void SDI_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* SDI driver bootstrap functions */

static int SDI_Available(void)
{
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
	if ((envr) && (SDL_strcmp(envr, SDIVID_DRIVER_NAME) == 0)) {
		return(1);
	}

	return 1;
}

static void SDI_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *SDI_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));
	
	L4_ThreadId_t server;
	objectid_t handle;
	while(GetObject("/video0", IF_FRAMEBUFFER_ID, &server, &handle) != OK) {
		LogMessage("Waiting for /video0");
		L4_Yield();
	}
	device->hidden->driver = server;
	device->hidden->card = handle;
	LogMessage("Found video card (TID: %lx OID: %lu)", device->hidden->driver.raw,
	           device->hidden->card);
	
	L4_ThreadId_t consoleserver;
	objectid_t consolehandle;           
	while(GetObject("/console0", IF_FILE_ID, &consoleserver, &consolehandle) != OK) {
		LogMessage("Waiting for /console0");
		L4_Yield();
	}
	device->hidden->consoleserver = consoleserver;
	device->hidden->consolehandle = consolehandle;

	/* Set the function pointers */
	device->VideoInit = SDI_VideoInit;
	device->ListModes = SDI_ListModes;
	device->SetVideoMode = SDI_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = SDI_SetColors;
	device->UpdateRects = SDI_UpdateRects;
	device->VideoQuit = SDI_VideoQuit;
	device->AllocHWSurface = SDI_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = SDI_LockHWSurface;
	device->UnlockHWSurface = SDI_UnlockHWSurface;
	device->FlipHWSurface = SDI_FlipHWSurface;
	device->FreeHWSurface = SDI_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = SDI_InitOSKeymap;
	device->PumpEvents = SDI_PumpEvents;

	device->freeme = SDI_DeleteDevice;

	return device;
}

VideoBootStrap SDI_bootstrap = {
	SDIVID_DRIVER_NAME, "SDL dummy video driver",
	SDI_Available, SDI_CreateDevice
};


int SDI_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	CORBA_Environment env = idl4_default_environment;
	L4_Fpage_t rcvwindow;
	idl4_fpage_t fp;
	idlsize_t fbsize = 0;
	
	L4_Word_t vmem = 0x90000000;
	rcvwindow = L4_Fpage(vmem, 0x8000000);
	idl4_set_rcv_window(&env, (L4_Fpage_t) L4_MapGrantItems(rcvwindow).raw);
	IF_FRAMEBUFFER_MapFrameBuffer(this->hidden->driver, this->hidden->card, &fp, &fbsize, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			SDL_SetError("MapFrameBuffer failed: IPC failed, code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			return -1;
		case CORBA_USER_EXCEPTION:
			SDL_SetError("MapFrameBuffer failed: User-defined exception");
			CORBA_exception_free(&env);
			return -1;
		case CORBA_NO_EXCEPTION:
			break;
	}
	
	this->hidden->framebuffer = ((char*) vmem);
	this->hidden->framebuffer_size = fbsize;
	printf("Mapped framebuffer at %p\n", this->hidden->framebuffer);

	/* Determine the screen depth (use default 8-bit depth) */
	/* we change this during the SDL_SetVideoMode implementation... */
	vformat->BitsPerPixel = 8;
	vformat->BytesPerPixel = 1;

	/* We're done! */
	return(0);
}

SDL_Rect **SDI_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
	// any mode possible...	
	return (SDL_Rect**) -1;
}

SDL_Surface *SDI_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	int real_width = 0, real_height = 0, real_bpp = 0;
	idlsize_t offset = 0, pitch = 0;
	uint32_t redmask = 0, greenmask = 0, bluemask = 0;
	
	CORBA_Environment env = idl4_default_environment;
	IF_FRAMEBUFFER_SetMode(this->hidden->driver, this->hidden->card, width, height, bpp, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			printf("IPC failed (SetMode), code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't set video mode (ipc failed code %d)", CORBA_exception_id(&env));
			return NULL;
		case CORBA_USER_EXCEPTION:
			printf("User-defined exception (SetMode)");
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't set video mode (user exception)");
			return NULL;
		case CORBA_NO_EXCEPTION:
			break;
	}
	
	// gathering infos...
	env = idl4_default_environment;
	IF_FRAMEBUFFER_GetInfos(this->hidden->driver, this->hidden->card,
	                        &real_width, &real_height, &real_bpp,
	                        &offset, &pitch,
                                &redmask, &greenmask, &bluemask, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			printf("IPC failed (GetInfos), code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't get video mode infos (ipc failed code %d)", CORBA_exception_id(&env));
			return NULL;
		case CORBA_USER_EXCEPTION:
			printf("User-defined exception (GetInfos)");
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't get video mode infos (user exception)");
			return NULL;
		case CORBA_NO_EXCEPTION:
			break;
	}

	assert(offset + height * pitch <= this->hidden->framebuffer_size);
	char* pixels = this->hidden->framebuffer + offset;

	SDL_memset(pixels, 0, height * pitch);

	/* Allocate the new pixel format for the screen */
	if (!SDL_ReallocFormat(current, real_bpp, redmask, greenmask, bluemask, 0) ) {
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* Set up the new mode framebuffer */
	current->flags = flags & (SDL_FULLSCREEN | SDL_DOUBLEBUF);
	this->hidden->w = current->w = real_width;
	this->hidden->h = current->h = real_height;
	current->pitch = pitch;
	current->pixels = pixels;
	
	SDI_UpdateRects(this, 0, NULL);

	/* We're done */
	return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int SDI_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void SDI_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int SDI_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void SDI_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

static void SDI_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	if(this->screen == NULL || this->screen->pixels == NULL)
		return;
 
	CORBA_Environment env = idl4_default_environment;
	IF_FRAMEBUFFER_Update(this->hidden->driver, this->hidden->card, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			LogMessage("IPC failed, code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			return;
		case CORBA_USER_EXCEPTION:
			LogMessage("User-defined exception");
			CORBA_exception_free(&env);
			return;
		case CORBA_NO_EXCEPTION:
			break;
	}
}

static int SDI_FlipHWSurface(_THIS, SDL_Surface *surface)
{
	if(this->screen == NULL || this->screen->pixels == NULL)
		return 0;
	
	CORBA_Environment env = idl4_default_environment;
	IF_FRAMEBUFFER_Update(this->hidden->driver, this->hidden->card, &env);
	switch (env._major) {
	case CORBA_SYSTEM_EXCEPTION:
		LogMessage("IPC failed, code %d\n", CORBA_exception_id(&env));
		CORBA_exception_free(&env);
		return -1;
	case CORBA_USER_EXCEPTION:
		LogMessage("User-defined exception");
		CORBA_exception_free(&env);
		return -1;
	case CORBA_NO_EXCEPTION:
		break;
	}

	return 0;
}

int SDI_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void SDI_VideoQuit(_THIS)
{
	this->screen->pixels = NULL;

	CORBA_Environment env = idl4_default_environment;
	IF_FRAMEBUFFER_SetMode(this->hidden->driver, this->hidden->card, 0, 0, 0, &env);
	switch (env._major) {
		case CORBA_SYSTEM_EXCEPTION:
			printf("IPC failed (SetMode), code %d\n", CORBA_exception_id(&env));
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't set video mode (ipc failed code %d)", CORBA_exception_id(&env));
			return;
		case CORBA_USER_EXCEPTION:
			printf("User-defined exception (SetMode)");
			CORBA_exception_free(&env);
			SDL_SetError("Couldn't set video mode (user exception)");
			return;
		case CORBA_NO_EXCEPTION:
			break;
	}	
}
