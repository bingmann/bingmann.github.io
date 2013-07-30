/*
 * SDLjump
 * (C) 2005 Juan Pedro Bol�ar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * menu.c
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

#include <stdarg.h> 
 
#include "sdljump.h"
#include "menu.h"
#include "game.h"
#include "setup.h"
#include "tools.h"

extern SDL_Surface * screen;
extern L_gblOptions gblOps;
 
/*
 * Usage:
 *  The first value must be the number of options passed. If, additionally, you
 *  want an explanatory text appearing on top of the menu, turn this value to
 *  negative.
 *      Examples: playMenu (3, "option1", "option2", "option3");
 *                playMenu (-2, "text", "option1", "option2");
 */

int playMenu(graphics_t* gfx, int nops, ...)
{
    va_list p; 
    char **options = NULL;
	char **tips = NULL;
    int i;
	int select = 0;
 
    va_start(p, nops);
   
    options = malloc(sizeof(char*)*abs(nops));
    if (nops < 0)
        tips = malloc(sizeof(char*)*abs(nops));
	
    for (i=0; i<abs(nops); i++) {
        options[i] = va_arg(p, char*);
		if (nops < 0)
			tips[i] = va_arg(p, char*);
	}
    va_end(p);
    
    select = playMenuTab(gfx, nops, options, tips);
	free(options);
	free(tips);
	return select;
}

int playMenuTab(graphics_t* gfx, int nops, char **options, char **tips)
{
    int done = FALSE;
    int prevselect = 0;
	int select = 0;
	int i;
    fader_t* faders;
    L_timer timer;
    
    initTimer(&timer, gblOps.fps);
	updateTimer(&timer);
	faders = malloc(sizeof(fader_t) * abs(nops));
	
	setFader(&faders[select], 0, gfx->hlalpha, 1);
	for (i=select+1; i<abs(nops); i++) {
		setFader(&faders[i],0,0,1);
	}
	
    drawMenu(gfx, abs(nops),options);
	if (nops < 0)
		drawTip(gfx,tips[select]);
	FlipScreen();
	
    while (!done) {
		updateTimer(&timer);
		for (i=0; i<abs(nops); i++) {
			updateFader(&faders[i], timer.ms);
		}
		prevselect = select;
        switch(checkMenuKeys()) {
            case ENTER:
                done = TRUE;
                break;
            case UP:
                select--;
                if (select < 0) select = abs(nops)-1;
                setFader(&faders[prevselect], faders[prevselect].value, 0, MENUFADE);
				setFader(&faders[select], gfx->hlalpha, gfx->hlalpha, MENUFADE);
				//setFader(&faders[select], faders[select].value, gfx->hlalpha, MENUFADE);
				if (nops < 0) {
					drawTip(gfx,tips[select]);
				}
                break;
            case DOWN:
                select++;
                if (select >= abs(nops)) select = 0;
                setFader(&faders[prevselect], faders[prevselect].value, 0, MENUFADE);
				setFader(&faders[select], gfx->hlalpha, gfx->hlalpha, MENUFADE);
				//setFader(&faders[select], faders[select].value, gfx->hlalpha, MENUFADE);
				if (nops < 0) {
					drawTip(gfx,tips[select]);
				}
                break;
            case BACK:
                select = NONE;
                done = TRUE;
                break;
            default:
                break;
        }
		for (i=0; i<abs(nops); i++) {
			updateFader(&faders[i],timer.ms);
			drawOption(gfx,  i, options[i], faders[i].value);
		}
		FlipScreen();
    }
	
	free(faders);
    return select;
}

void drawTip (graphics_t* gfx, char* tip)
{
	SDL_Rect rect;
	rect.x = gfx->tipX;
	rect.y = gfx->tipY;
	rect.w = gfx->tipW;
	rect.h = gfx->tipH;
	JPB_PrintSurface(gfx->menuBg,&rect,&rect);
	SFont_WriteAligned(gfx->tipfont, gfx->tipX, gfx->tipY, gfx->tipW,0, gfx->tAlign, tip);
}

void drawMenu(graphics_t* gfx, int nops, char** ops)
{
    int i;
    
    JPB_PrintSurface(gfx->menuBg,NULL,NULL);

    if (nops > 0)
        drawOption(gfx, 0,ops[0],ON);
    for (i=1; i<nops; i++) {
        drawOption(gfx, i,ops[i],OFF);
    }
}

void drawOption(graphics_t* gfx,  int opt, char* option, int alpha)
{
	SDL_Rect rect;

	rect.x = gfx->menuX;
	rect.y = gfx->menuY + opt * SFont_TextHeight(gfx->menufont) + gfx->menuGap * opt;
	rect.w = gfx->menuW;
	rect.h = SFont_TextHeight(gfx->menufont);
	
	JPB_PrintSurface(gfx->menuBg, &rect, &rect);
    JPB_drawSquare(gfx->hlcolor, alpha, rect.x,rect.y,rect.w,rect.h);
    
    //SFont_Write(gfx->menufont, x, y, option);
	SFont_WriteMaxWidth (gfx->menufont, rect.x, rect.y, rect.w, gfx->mAlign, "...", option);

}

int checkMenuKeys()
{
    SDL_Event event;
    int ret = IDLE;
	
    while( SDL_PollEvent( &event ) ){
        switch( event.type ){
            /* A key is pressed */
            case SDL_KEYDOWN: 
                if( event.key.keysym.sym == SDLK_RETURN){
                    ret = ENTER;
                }
                if( event.key.keysym.sym == SDLK_UP){
                    ret = UP;
                }
                if( event.key.keysym.sym == SDLK_DOWN){
                    ret = DOWN;
                }
                if( event.key.keysym.sym == SDLK_ESCAPE ){
                    ret = BACK;
                }
                break;
            /* A key UP. */
            case SDL_KEYUP:
                break;
            /* Quit: */
            case SDL_QUIT:
                break;
            /* Default */
            default:
                break;
        }
    }
    
    return ret;
}

void mainMenu(graphics_t* gfx)
{
    int opt;
    int r;
    int done = FALSE;
    
    while (!done) {
        opt = playMenu(gfx,-5,
            gfx->msg[msg_newgame], gfx->tip[tip_newgame],
            gfx->msg[msg_options], gfx->tip[tip_options],
			gfx->msg[msg_highscores], gfx->tip[tip_highscores],
			gfx->msg[msg_themes], gfx->tip[tip_themes],
            gfx->msg[msg_quit], gfx->tip[tip_quit]
			);
            
        switch(opt) {
        case 0:
            r = playMenu(gfx,-5,
            gfx->msg[msg_1playergame], gfx->tip[tip_1playergame],
            gfx->msg[msg_2playergame], gfx->tip[tip_2playergame],
            gfx->msg[msg_3playergame], gfx->tip[tip_3playergame],
            gfx->msg[msg_4playergame], gfx->tip[tip_4playergame],
            gfx->msg[msg_back], gfx->tip[tip_back]);
            if (NONE < r && r < MAX_PLAYERS) {
				while (playGame(gfx, r+1))
				SDL_Delay(1);
			}
            break;
        case 1:
            optionsMenu(gfx);
            break;
		case 2:
			drawRecords(gfx,gblOps.records);
			pressAnyKey ();
			break;
		case 3:
			themeMenu(gfx);
			break;
        case 4:
            done = TRUE;
            break;
        case NONE:
             done = TRUE;
             break;
        default:
                break;  
        }
    }
}

void optionsMenu(graphics_t* gfx)
{
    int opt;
    int done = FALSE;
    
    while (!done) {
        opt = playMenu(gfx,-5,
            gfx->msg[msg_gameoptions], gfx->tip[tip_gameoptions],
            gfx->msg[msg_graphicoptions], gfx->tip[tip_graphicoptions],
            gfx->msg[msg_playernames], gfx->tip[tip_playernames],
            gfx->msg[msg_redefinekeys], gfx->tip[tip_redefinekeys],
            gfx->msg[msg_back], gfx->tip[tip_back]);
            
        switch(opt) {
            case 0:
                gameOptionsMenu(gfx);
                break;
            case 1:
                gfxOptionsMenu(gfx);
                break;
            case 2:
                playersNameMenu(gfx);
                break;
            case 3:
                redefineKeysMenu(gfx);
                break;
            case 4:
                done = TRUE;
                break;
            case NONE:
                done = TRUE;
                break;
            default:
                break;  
        }
    }
}

void playersNameMenu(graphics_t* gfx)
{
    int done = FALSE;
    int  r;
    char* tmpstr = NULL;
    
    while (!done) {
       r = playMenu(gfx, 5,
            gfx->msg[msg_player1name],
            gfx->msg[msg_player2name],
            gfx->msg[msg_player3name],
            gfx->msg[msg_player4name],
            gfx->msg[msg_back]);
        if (r == NONE || r == 4) {
            done = TRUE;
            break;
        }
        tmpstr = inputMenu (gfx, gfx->tip[tip_inputname], 
				gblOps.pname[r], 14);
        if (tmpstr != NULL) {
            free(gblOps.pname[r]);
            gblOps.pname[r] = tmpstr;
        }
    }
}

void redefineKeysMenu(graphics_t* gfx)
{
    int done = FALSE;
    int done2 = FALSE;
	int done3 = FALSE;
    int opt, r;
    SDL_Event event;
    
    while (!done) {
        r = playMenu(gfx,5,
            gfx->msg[msg_player1keys],
            gfx->msg[msg_player2keys],
            gfx->msg[msg_player3keys],
            gfx->msg[msg_player4keys],
            gfx->msg[msg_back]);
        if (r == NONE || r == 4) {
            done = TRUE;
            break;
        }
        while (!done2) {
            opt = playMenu(gfx,-4,
                gfx->msg[msg_redefkeyleft], gfx->tip[tip_redefkeyleft],
                gfx->msg[msg_redefkeyright], gfx->tip[tip_redefkeyright],
                gfx->msg[msg_redefkeyup],gfx->tip[tip_redefkeyup],
                gfx->msg[msg_back], gfx->tip[tip_back]);
            
            if (opt == NONE || opt == 3) {
                done2 = TRUE;
                break;
            }
            drawMenu(gfx, 0, NULL);
			/* Find key */
            while( !done3 ) {
                while (SDL_PollEvent( &event )) {
                	if( event.type == SDL_KEYDOWN){
                    	gblOps.keys[r][opt] = event.key.keysym.sym;
						done3 = TRUE;
                	}
				}
            }
        }
        done2 = FALSE;
    }
}

char* inputMenu (graphics_t* gfx, char* tip, char* inittext, int maxWidth)
{
    SDL_Event event;
	SDL_Rect rect;
    char ch = '\0';
    char text[MAX_CHAR];
    char* retText = NULL;
    int len;
    int prevUnic;
    
    drawMenu(gfx, 0, NULL);
    
    if (tip != NULL) 
		drawTip(gfx,tip);
            
    sprintf(text,"%s",inittext);
    len = strlen(text);
    text[len] = '|';
    text[len+1] = '\0';
    
	rect.x = gfx->menuX;
	rect.y = gfx->menuY;
	rect.w = gfx->menuW;
	rect.h = SFont_AlignedHeight(gfx->menufont, gfx->menuW,0, text);
	
    //JPB_drawSquare(gfx->hlcolor, gfx->hlalpha, rect.x, rect.y, rect.w,rect.h);
    SFont_WriteAligned(gfx->menufont, rect.x, rect.y, gfx->menuW,0, ACENTER, text);
    FlipScreen();   
    
    prevUnic=SDL_EnableUNICODE(TRUE);
    while (ch != SDLK_RETURN) {
        if (event.type == SDL_KEYDOWN) {
            ch=event.key.keysym.unicode;
            
			rect.h = SFont_AlignedHeight(gfx->menufont, gfx->menuW,0, text);
            JPB_PrintSurface(gfx->menuBg,&rect, &rect);
            
            if ( (ch>31) || (ch=='\b')) {
                if ((ch=='\b')&&(strlen(text)>0)) {
                    len = strlen(text);
                    text[strlen(text)-2]='|';
                    text[strlen(text)-1]='\0';
                } else {
                    len = strlen(text);
                    text[len-1] = ch;
                    text[len] = '|';
                    text[len+1] = '\0';
                }
            }
            if (strlen(text)>maxWidth) 
                text[maxWidth]='\0';
                
            //JPB_drawSquare(gfx->hlcolor, gfx->hlalpha, rect.x, rect.y, rect.w,rect.h);
            SFont_WriteAligned(gfx->menufont, rect.x, rect.y, gfx->menuW,0, ACENTER, text);
            FlipScreen();
        }
        SDL_WaitEvent(&event);
        SDL_PollEvent(&event);
    }
    SDL_EnableUNICODE(prevUnic);
    text[strlen(text)-1]='\0';
    if ((retText = malloc(sizeof(char)*(strlen(text)+1))) == NULL)
        return NULL;
    strcpy(retText, text);
    
    return retText;
}

void gameOptionsMenu(graphics_t* gfx)
{
    int opt;
    int r;
    int done = FALSE;
    
    while (!done) {
        opt = playMenu(gfx,-5,
            gfx->msg[msg_fpslimit], gfx->tip[tip_fpslimit],
            gfx->msg[msg_jumpingrot], gfx->tip[tip_jumpingrot],
            gfx->msg[msg_scrollmode], gfx->tip[tip_scrollmode],
            gfx->msg[msg_mplives], gfx->tip[tip_mplives],
            gfx->msg[msg_back], gfx->tip[tip_back]);
            
        switch(opt) {
            case 0:
                r = playMenu(gfx,-4,
                    gfx->msg[msg_40fps], gfx->tip[tip_40fps],
                    gfx->msg[msg_100fps], gfx->tip[tip_100fps],
                    gfx->msg[msg_300fps], gfx->tip[tip_300fps],
					gfx->msg[msg_nolimit], gfx->tip[tip_nolimit]);
                if (r == 0) 
                    gblOps.fps = 40;
                else if (r == 1)
                    gblOps.fps = 100;
                else if (r == 2)
                    gblOps.fps = 300;
				else if (r == 3)
                    gblOps.fps = 0;

                break;
            case 1:
                r = playMenu(gfx,-4,
                    gfx->msg[msg_norot], gfx->tip[tip_norot],
                    gfx->msg[msg_orginalrot], gfx->tip[tip_orginalrot],
                    gfx->msg[msg_fullrot], gfx->tip[tip_fullrot],
                    gfx->msg[msg_fullrotaa], gfx->tip[tip_fullrotaa]);
                if (r == 0)
                    gblOps.rotMode = ROTNONE;
                else if (r == 1)
                    gblOps.rotMode = ROTORIG;
                else if (r == 2 || r == 3)
                    gblOps.rotMode = ROTFULL;
                break;
            case 2:
                r = playMenu(gfx,-3,
                    gfx->msg[msg_hardscroll], gfx->tip[tip_hardscroll],
                    gfx->msg[msg_softscroll], gfx->tip[tip_softscroll],
                    gfx->msg[msg_back], gfx->tip[tip_back]);
                if (r == 0) 
                    gblOps.scrollMode = HARDSCROLL;
                else if (r == 1)
                    gblOps.scrollMode = SOFTSCROLL;
                break;
            case 3:
                r = playMenu(gfx,5, "1", "2", "3", "4", "5");
                if (r >= 0) 
                    gblOps.mpLives = ++r;
                break;
            case 4:
                done = TRUE;
                break;
            case NONE:
                done = TRUE;
                break;
            default:
                break;  
        }
    }  
}

void gfxOptionsMenu(graphics_t* gfx)
{
    int opt;
    int r;
    int done = FALSE;
    
    while (!done) {
        opt = playMenu(gfx,-4,
            gfx->msg[msg_opengl], gfx->tip[tip_opengl],
            gfx->msg[msg_bpp], gfx->tip[tip_bpp],
            gfx->msg[msg_fullscreen], gfx->tip[tip_fullscreen],
            gfx->msg[msg_back], gfx->tip[tip_back]);
            
        switch(opt) {
            case 0:
                r = playMenu(gfx,-2,
                    gfx->msg[msg_no], gfx->tip[tip_no],
                    gfx->msg[msg_yes], gfx->tip[tip_yes]);
                resetEngine(gfx);
                break;
            case 1:
                r = playMenu(gfx,-4,
                    gfx->msg[msg_8bpp], gfx->tip[tip_8bpp],
                    gfx->msg[msg_16bpp], gfx->tip[tip_16bpp],
                    gfx->msg[msg_24bpp], gfx->tip[tip_24bpp],
                    gfx->msg[msg_32bpp], gfx->tip[tip_32bpp]);
                if (r != NONE)
                    gblOps.bpp = (r+1)*8;
                resetEngine(gfx);
                break;
            case 2:
                r = playMenu(gfx,-2,
                    gfx->msg[msg_no], gfx->tip[tip_no],
                    gfx->msg[msg_yes], gfx->tip[tip_yes]);
                if (r != NONE)
                    gblOps.fullsc = r;
                resetEngine(gfx);
                break;
            case 3:
                done = TRUE;
                break;
            case NONE:
                done = TRUE;
                break;
            default:
                break;  
        }
    }  
}

void themeMenu(graphics_t* gfx)
{
    int opt;
    int done = FALSE;
    
    while (!done) {
        opt = playMenu(gfx,-3,
		    gfx->msg[msg_choostheme], gfx->tip[tip_choostheme],
            gfx->msg[msg_themefolders], gfx->tip[tip_themefolders],
            gfx->msg[msg_back], gfx->tip[tip_back]);
            
        switch(opt) {
        case 0:
			chooseThemeMenu(gfx);
            break;
        case 1:
			themeDirsMenu(gfx);
            break;
        case 2:
			done = TRUE;
            break;
        case NONE:
            done = TRUE;
            break;
        default:
            break;  
        }
    }  
}

void chooseThemeMenu(graphics_t* gfx)
{
	char **options = NULL;
	char **tips = NULL;
	char **dirs = NULL;
	char **buf = NULL;
	char **fullpaths = NULL;
	int *index = NULL;

	char* str = NULL;
	int nf = 0, n = 0, no = 0;
	int i = 0;
	int r;
	
	for (i=0; i < gblOps.ntfolders; i++) {
		n = getDirList(gblOps.themeDirs[i], &buf);
		sumStringTabs(&dirs, nf, buf, n);
		nf = sumStringTabs_Cat(&fullpaths, nf, buf, n, gblOps.themeDirs[i]);
		free(buf);
		buf = NULL;
	}
	
	no = 0;
	for (i = 0; i < nf; i++) {
		str = getThemeComment(fullpaths[i]);
		if (str != NULL) {
			no++;
			
			index = realloc(index, sizeof(int)*no);
			tips = realloc(tips, sizeof(char*)*no);
			options = realloc(options, sizeof(char*)*no);
			
			tips[no-1] = str;
			options[no-1] = dirs[i];
			index[no-1] = i;
		}
	}

	r = playMenuTab(gfx,-no, options, tips);
	if (r >= 0 && r < no) {
		gblOps.dataDir = realloc(gblOps.dataDir, 
			sizeof(char)* (strlen( fullpaths[index[r]] )+1) );
		strcpy(gblOps.dataDir, fullpaths[index[r]]);
		resetEngine(gfx);
	}
	
	for (i = 0; i < nf; i++) {
		free(dirs[i]);
		free(fullpaths[i]);
	}
	for (i = 0; i< no; i++) {
		free(tips[i]);
	}
	free(tips);
	free(options);
	free(index);
	free(dirs);
	free(fullpaths);
}

void themeDirsMenu(graphics_t* gfx)
{
	char **options = NULL;
	char **tips = NULL;
	char* buf = NULL;
	int i;
	int done = FALSE;
	int opt, opt2;
	
	while (!done) {
		options = malloc(sizeof(char*)* (gblOps.ntfolders+2));
		tips = malloc(sizeof(char*)* (gblOps.ntfolders+2));
		
		options[0] = gfx->msg[msg_addthemefolder];
		options[gblOps.ntfolders+1] = gfx->msg[msg_back];
		tips[0] = gfx->tip[tip_addthemefolder];
		tips[gblOps.ntfolders+1] = gfx->tip[tip_back];
		
		for (i=0; i< gblOps.ntfolders; i++){
			options[i+1] = gblOps.themeDirs[i];
			tips[i+1] = gfx->tip[tip_themefolder];
		}
	
		opt = playMenuTab(gfx, -(gblOps.ntfolders+2), options, tips);
		if (opt == 0) {
			gblOps.ntfolders++;
			gblOps.themeDirs = realloc(gblOps.themeDirs, sizeof(char*)*gblOps.ntfolders); 
			buf = getcwd(NULL,0);
			gblOps.themeDirs[gblOps.ntfolders-1] =  
				inputMenu(gfx,gfx->tip[tip_writefolder],buf,gfx->menuW);
			free(buf); buf = NULL;
		} else if (opt > 0 && opt < gblOps.ntfolders+1) {
			opt2 = playMenu(gfx, -3,
				gfx->msg[msg_editfolder], gfx->tip[msg_editfolder],
				gfx->msg[msg_deletefolder], gfx->tip[tip_deletefolder],
				gfx->msg[msg_back], gfx->tip[tip_back]);
			switch (opt2) {
			case 0:
				buf = gblOps.themeDirs[opt-1];
				gblOps.themeDirs[opt-1] = inputMenu(gfx,gfx->tip[tip_writefolder],buf, 256);
				free(buf);
				break;
			case 1:
				buf = gblOps.themeDirs[opt-1];
				for (i = opt; i < gblOps.ntfolders; i++) {
					gblOps.themeDirs[i-1] = gblOps.themeDirs[i];
				}
				free(buf); buf = NULL;
				gblOps.ntfolders--;
				gblOps.themeDirs = realloc(gblOps.themeDirs, sizeof(char*)*gblOps.ntfolders);
				break;
			default:
				break;
			}
		} else {
			done = TRUE;
		}

		free(options);
		free(tips);
	}
}
