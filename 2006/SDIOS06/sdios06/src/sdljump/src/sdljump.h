/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolívar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * sdljump.h
 */

/*
    Copyright (C) 2003-2004, Juan Pedro Bolivar Puente

    SDLjump is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    SDLjump is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDLjump; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _SDLJUMP_H_
#define _SDLJUMP_H_

// defines from autoconf

#ifdef CONFIG_ARCH_IA32
#define DATA_PREFIX		"/"
#define PACKAGE			"sdljump"
#endif

//==============================================================================
// HEADERS
//==============================================================================

/* ---------------------------- STANDARD INCLUDES ----------------------------*/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>

/* --------------------------- EXTERNAL LIBRARIES ----------------------------*/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/* ----------------------------- LOCAL INCLUDES ------------------------------*/
#include "surface.h"
#include "sprite.h"
#include "SFont.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//==============================================================================
// DEFINITIONS
//==============================================================================

/* Config file versions */
#define PROT_VERS "0.7"
/* Theme file format version */
#define THEME_VERS "0.2"

#ifndef HAVE_CONFIG_H
#define VERSION "0.91"
#endif

/* Files */
#define THEMEFILE "/config.theme"
#define DEFOLDER "default"
#define CFGFILE "/sdljump.cfg"
#define HSCFILE "/sdljump.hsc"

#define MAX_THEMEFOLDERS 5

#define MAX_CHAR 512

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

/* Hero rotation modes */
#define ROTFULL 1
#define ROTORIG 2
#define ROTNONE 0

/* Scrolling modes */
#define HARDSCROLL 0
#define SOFTSCROLL 1

/* Maximun number of players */
#define MAX_PLAYERS 4

enum {
	H_STAND, H_WALK, H_JUMP, HEROANIMS
};

/* The number of records */
#define MAX_RECORDS 10

/* Be careful, these values are not arbitrary */
#define RIGHT 1
#define LEFT 0

/* Delay and rate in original mode*/
#define DELAY 25
#define FPSRATE 40

/* Keys ids */
#define KEYS 3
#define LEFTK 0
#define RIGHTK 1
#define JUMPK 2

/* Default keys */
#define KEY_QUIT SDLK_ESCAPE

#define KEY_LEFT1 SDLK_LEFT
#define KEY_LEFT2 SDLK_a
#define KEY_LEFT3 SDLK_j
#define KEY_LEFT4 SDLK_KP4

#define KEY_RIGHT1 SDLK_RIGHT
#define KEY_RIGHT2 SDLK_d
#define KEY_RIGHT3 SDLK_l
#define KEY_RIGHT4 SDLK_KP6

#define KEY_UP1 SDLK_UP
#define KEY_UP2 SDLK_w
#define KEY_UP3 SDLK_i
#define KEY_UP4 SDLK_KP8

/* Default player name */
#define PNAME "Player"

/* Some code switchs */
// #define DEVEL
#define GLFINISH


enum {
	txt_name,
	txt_floor, 
	txt_mode,
	txt_time,
	txt_hscnote,
	txt_newhsc,
	txt_gameover,
	txt_askquit,
	txt_pause,
	TXT_COUNT
};

enum {
	msg_newgame,
	msg_options,
	msg_highscores,
	msg_themes,
	msg_quit,
	msg_back,
	msg_themefolders,
	msg_choostheme,
	msg_addthemefolder,
	msg_deletefolder,
	msg_editfolder,
	msg_1playergame,
	msg_2playergame,
	msg_3playergame,
	msg_4playergame,
	msg_gameoptions,
	msg_graphicoptions,
	msg_playernames,
	msg_redefinekeys,
	msg_fpslimit,
	msg_jumpingrot,
	msg_scrollmode,
	msg_40fps,
	msg_100fps,
	msg_300fps,
	msg_nolimit,
	msg_norot,
	msg_orginalrot,
	msg_fullrot,
	msg_fullrotaa,
	msg_softscroll,
	msg_hardscroll,
	msg_mplives,
	msg_opengl,
	msg_bpp,
	msg_fullscreen,
	msg_8bpp,
	msg_16bpp,
	msg_24bpp,
	msg_32bpp,
	msg_yes,
	msg_no,
	msg_player1name,
	msg_player2name,
	msg_player3name,
	msg_player4name,
	msg_player1keys,
	msg_player2keys,
	msg_player3keys,
	msg_player4keys,
	msg_redefkeyleft,
	msg_redefkeyright,
	msg_redefkeyup,
	MSG_COUNT
};

enum {
	tip_newgame,
	tip_options,
	tip_highscores,
	tip_themes,
	tip_quit,
	tip_back,
	tip_themefolders,
	tip_choostheme,
	tip_addthemefolder,
	tip_themefolder,
	tip_deletefolder,
	tip_editfolder,
	tip_writefolder,
	tip_1playergame,
	tip_2playergame,
	tip_3playergame,
	tip_4playergame,
	tip_gameoptions,
	tip_graphicoptions,
	tip_playernames,
	tip_redefinekeys,
	tip_fpslimit,
	tip_jumpingrot,
	tip_scrollmode,
	tip_mplives,
	tip_40fps,
	tip_100fps,
	tip_300fps,
	tip_nolimit,
	tip_norot,
	tip_orginalrot,
	tip_fullrot,
	tip_fullrotaa,
	tip_softscroll,
	tip_hardscroll,
	tip_opengl,
	tip_bpp,
	tip_fullscreen,
	tip_8bpp,
	tip_16bpp,
	tip_24bpp,
	tip_32bpp,
	tip_yes,
	tip_no,
	tip_redefkeyleft,
	tip_redefkeyright,
	tip_redefkeyup,
	tip_inputname,
	TIP_COUNT
};

//==============================================================================
// LOCAL TYPES
//==============================================================================

/*
 * A record entry
 */
typedef struct record
{
	char* pname;
	int floor;
	char* mode;
	int time;
} records_t;

/*
 * Global Options
 */
typedef struct options
{
    int w;
    int h;
    int bpp;
    int fullsc;
    
    int fps;
    int rotMode;
    int scrollMode;
    int mpLives;
    
    /* Players */
    int keys[MAX_PLAYERS][KEYS];
    char* pname[MAX_PLAYERS];
    
    /* Data files */
    char *dataDir;
	char **themeDirs;
	int ntfolders;
	
	/* The records tab, organized from best to worst */
	records_t records[MAX_RECORDS]; 
}L_gblOptions;

/*
 * The theme data structure
 */
typedef struct graphix
{
    L_spriteDataRot* heroSprite[MAX_PLAYERS][HEROANIMS];

	/* Menu */
	JPB_surface* menuBg;
	int menuX;
	int menuY;
	int menuW;
	int menuH;
	int menuGap;
	int tipX;
	int tipY;
	int tipW;
	int tipH;
	int mAlign;
	int tAlign;
    SFont_Font* menufont; /* menu font*/
    SFont_Font* tipfont;
    Uint32 hlcolor;
	Uint8 hlalpha;
    
    /* In game screen*/
    JPB_surface* gameBg;
    JPB_surface* livePic;
    int liveAlign;
    int gameX;
    int gameY;
    int scoreX[MAX_PLAYERS];
    int scoreY[MAX_PLAYERS];
    int livesX[MAX_PLAYERS];
    int livesY[MAX_PLAYERS];
    SFont_Font* scorefont;
	SFont_Font* textfont;
	Uint32 gcolor;
	Uint8 galpha;
	
    JPB_surface* floorL;
    JPB_surface* floorR;
    JPB_surface* floorC;
	
	char* msg[MSG_COUNT];
	char* tip[TIP_COUNT];
	char* txt[TXT_COUNT];
} graphics_t;


#endif //_SDLJUMP_H_
