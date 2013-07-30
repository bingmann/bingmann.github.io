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
#include "SDL_config.h"

/* Being a null driver, there's no event stream. We just define stubs for
   most of the API. */

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_sdivideo.h"
#include "SDL_sdievents_c.h"

#include <assert.h>

#include <sdi/log.h>

#include <idl4glue.h>
#include <if/iffile.h>

#define KEYCOUNT 256
static SDLKey keymap[KEYCOUNT];

static void initkeymaps()
{
	static const int EXTENDED = 0x80;

	memset(keymap, 0, sizeof(keymap));

	keymap[0x01] = SDLK_ESCAPE;

	keymap[0x02] = SDLK_1;
	keymap[0x03] = SDLK_2;
	keymap[0x04] = SDLK_3;
	keymap[0x05] = SDLK_4;
	keymap[0x06] = SDLK_5;
	keymap[0x07] = SDLK_6;
	keymap[0x08] = SDLK_7;
	keymap[0x09] = SDLK_8;
	keymap[0x0A] = SDLK_9;
	keymap[0x0B] = SDLK_0;

	keymap[0x0e] = SDLK_BACKSPACE;
	keymap[0x0f] = SDLK_TAB;
	keymap[0x1c] = SDLK_RETURN;
	keymap[0x1d] = SDLK_LCTRL;
	keymap[0x2a] = SDLK_LSHIFT;
	keymap[0x29] = SDLK_CARET;
	keymap[0x36] = SDLK_RSHIFT;
	keymap[0x38] = SDLK_LALT;
	keymap[0x39] = SDLK_SPACE;

	keymap[0x45] = SDLK_NUMLOCK;
	keymap[EXTENDED | 0x35] = SDLK_KP_DIVIDE;
	keymap[0x37] = SDLK_KP_MULTIPLY;
	keymap[0x4a] = SDLK_KP_MINUS;
	keymap[0x47] = SDLK_KP7;
	keymap[0x48] = SDLK_KP8;
	keymap[0x49] = SDLK_KP9;
	keymap[0x4e] = SDLK_KP_PLUS;
	keymap[0x4b] = SDLK_KP4;
	keymap[0x4c] = SDLK_KP5;
	keymap[0x4d] = SDLK_KP6;
	keymap[0x4f] = SDLK_KP1;
	keymap[0x50] = SDLK_KP2;
	keymap[0x51] = SDLK_KP3;
	keymap[0x52] = SDLK_KP0;
	keymap[0x53] = SDLK_KP_PERIOD;
	keymap[EXTENDED | 0x1c] = SDLK_KP_ENTER;

	keymap[0x1E] = SDLK_a;
	keymap[0x30] = SDLK_b;
	keymap[0x2E] = SDLK_c;
	keymap[0x20] = SDLK_d;
	keymap[0x12] = SDLK_e;
	keymap[0x21] = SDLK_f;
	keymap[0x22] = SDLK_g;
	keymap[0x23] = SDLK_h;
	keymap[0x17] = SDLK_i;
	keymap[0x24] = SDLK_j;
	keymap[0x25] = SDLK_k;
	keymap[0x26] = SDLK_l;
	keymap[0x32] = SDLK_m;
	keymap[0x31] = SDLK_n;
	keymap[0x18] = SDLK_o;
	keymap[0x19] = SDLK_p;
	keymap[0x10] = SDLK_q;
	keymap[0x13] = SDLK_r;
	keymap[0x1F] = SDLK_s;
	keymap[0x14] = SDLK_t;
	keymap[0x16] = SDLK_u;
	keymap[0x2F] = SDLK_v;
	keymap[0x11] = SDLK_w;
	keymap[0x2D] = SDLK_x;
	keymap[0x15] = SDLK_y;
	keymap[0x2C] = SDLK_z;

	keymap[EXTENDED | 0x1d] = SDLK_RCTRL;
	keymap[EXTENDED | 0x38] = SDLK_RALT;

	keymap[EXTENDED | 0x47] = SDLK_HOME;
	keymap[EXTENDED | 0x48] = SDLK_UP;
	keymap[EXTENDED | 0x49] = SDLK_PAGEUP;
	keymap[EXTENDED | 0x4b] = SDLK_LEFT;
	keymap[EXTENDED | 0x4d] = SDLK_RIGHT;
	keymap[EXTENDED | 0x4f] = SDLK_END;
	keymap[EXTENDED | 0x50] = SDLK_DOWN;	
	keymap[EXTENDED | 0x51] = SDLK_PAGEDOWN;
	keymap[EXTENDED | 0x52] = SDLK_INSERT;
	keymap[EXTENDED | 0x53] = SDLK_DELETE;
}

static void TranslateKey(int scancode, SDL_keysym *keysym)
{
    /* Set the keysym information */
    keysym->scancode = scancode;
    keysym->sym = keymap[scancode];
    keysym->mod = KMOD_NONE;

    /* If UNICODE is on, get the UNICODE value for the key */
    keysym->unicode = 0;
    // TODO translate into unicode
}

static void keyinput(_THIS)
{
	// prepare a buffer for the content to read from the console
	unsigned char buffer[256];
	static int extended = 0;
	int pressed;
	int scancode;
	SDL_keysym keysym;
	buffer_t inbuf;
	size_t i;
	CORBA_Environment env = idl4_default_environment;
	
	inbuf._buffer = (CORBA_char*) buffer;
	inbuf._length = 0;
	inbuf._maximum = sizeof(buffer);
	
	
	IF_FILE_Read(this->hidden->consoleserver, this->hidden->consolehandle,
	             0, sizeof(buffer), &inbuf, &env);
	if(env._major != CORBA_NO_EXCEPTION)
		LogMessage("exception while reading from console");
		
	for ( i=0; i<inbuf._length; ++i ) {
		if(buffer[i] == 0xe0) {
			extended = 1;
			continue;
		}
		
		scancode = buffer[i] & 0x7F;
		if(extended) {
			scancode |= 0x80;
			extended = 0;
		}
		if ( buffer[i] & 0x80 ) {
			pressed = SDL_RELEASED;
		} else {
			pressed = SDL_PRESSED;
		}
	    TranslateKey(scancode, &keysym);
		SDL_PrivateKeyboard(pressed, &keysym);
	}
}

void SDI_PumpEvents(_THIS)
{
	keyinput(this);
}

void SDI_InitOSKeymap(_THIS)
{
	initkeymaps();
}

/* end of SDL_nullevents.c ... */

