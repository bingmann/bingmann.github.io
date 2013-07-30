/*
 * SDLjump
 * (C) 2005 Juan Pedro Bol�ar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * menu.h
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

#ifndef _MENU_H_
#define _MENU_H_

#define MENUFADE 1000

#define NONE -1
#define IDLE 0
#define UP 1
#define DOWN 2
#define ENTER 3
#define BACK 4

/*
 * Usage:
 *  The first value must be the number of options passed. If, additionally, you
 *  want an explanatory text appearing on top of the menu, turn this value to
 *  negative.
 *      Examples: playMenu (3, "option1", "option2", "option3");
 *                playMenu (-2, "text", "option1", "option2");
 */
int playMenu(graphics_t* gfx, int nops, ...);

int playMenuTab(graphics_t* gfx, int nops, char **options, char **tips);

void drawTip (graphics_t* gfx, char* tip);

int checkMenuKeys();

void drawMenu(graphics_t* gfx, int nops, char** ops);

void drawOption(graphics_t* gfx,  int opt, char* option, int alpha);

void mainMenu(graphics_t* gfx);

void optionsMenu(graphics_t* gfx);

void gfxOptionsMenu(graphics_t* gfx);

void redefineKeysMenu(graphics_t* gfx);

void playersNameMenu(graphics_t* gfx);

char* inputMenu (graphics_t* gfx, char* txt, char* inittext, int maxWidth);

void gameOptionsMenu(graphics_t* gfx);

void themeMenu(graphics_t* gfx);

void themeDirsMenu(graphics_t* gfx);

void chooseThemeMenu(graphics_t* gfx);

#endif //_MENU_H_
