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

#ifndef _SDL_config_minimal_h
#define _SDL_config_minimal_h

#include "SDL_platform.h"

/* This is the minimal configuration that can be used to build SDL */

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* Enable the dummy audio driver (src/audio/dummy/\*.c) */
#define SDL_AUDIO_DRIVER_DUMMY	1

/* Enable the stub cdrom driver (src/cdrom/dummy/\*.c) */
#define SDL_CDROM_DISABLED	1

/* Enable the stub joystick driver (src/joystick/dummy/\*.c) */
#define SDL_JOYSTICK_DISABLED	1

/* Enable the stub shared object loader (src/loadso/dummy/\*.c) */
#define SDL_LOADSO_DISABLED	1

/* Enable the stub thread support (src/thread/generic/\*.c) */
#define SDL_THREADS_DISABLED	1

#define SDL_TIMER_SDI	1

/* Enable the dummy video driver (src/video/dummy/\*.c) */
#define SDL_VIDEO_DRIVER_SDI	1

// Headers
#define HAVE_STDLIB_H	1
#define HAVE_STDARG_H	1
#define HAVE_MALLOC_H	1
#define HAVE_STRING_H	1
#define HAVE_STDINT_H	1
#define HAVE_STDIO_H	1

#define HAVE_MALLOC
//#undef HAVE_CALLOC
#define HAVE_REALLOC
#define HAVE_FREE
//#undef HAVE_ALLOCA
#undef HAVE_GETENV
#undef HAVE_PUTENV
//#undef HAVE_UNSETENV
//#undef HAVE_QSORT
//#undef HAVE_ABS
//#undef HAVE_BCOPY
#define HAVE_MEMSET
#define HAVE_MEMCPY
//#undef HAVE_MEMMOVE
//#undef HAVE_MEMCMP
#define HAVE_STRLEN
//#undef HAVE_STRLCPY
//#undef HAVE_STRLCAT
//#undef HAVE_STRDUP
//#undef HAVE__STRREV
//#undef HAVE__STRUPR
//#undef HAVE__STRLWR
//#undef HAVE_INDEX
//#undef HAVE_RINDEX
#define HAVE_STRCHR
//#undef HAVE_STRRCHR
//#undef HAVE_STRSTR
//#undef HAVE_ITOA
//#undef HAVE__LTOA
//#undef HAVE__UITOA
//#undef HAVE__ULTOA
//#undef HAVE_STRTOL
//#undef HAVE_STRTOUL
//#undef HAVE__I64TOA
//#undef HAVE__UI64TOA
//#undef HAVE_STRTOLL
//#undef HAVE_STRTOULL
//#undef HAVE_STRTOD
//#undef HAVE_ATOI
//#undef HAVE_ATOF
#define HAVE_STRCMP
#define HAVE_STRNCMP
//#undef HAVE__STRICMP
//#undef HAVE_STRCASECMP
//#undef HAVE__STRNICMP
//#undef HAVE_STRNCASECMP
//#undef HAVE_SSCANF
#define HAVE_SNPRINTF
#define HAVE_VSNPRINTF
//#undef HAVE_ICONV
//#undef HAVE_SIGACTION
//#undef HAVE_SETJMP
//#undef HAVE_NANOSLEEP
//#undef HAVE_CLOCK_GETTIME
//#undef HAVE_DLVSYM

#endif
