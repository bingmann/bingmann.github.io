/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolíar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * setup.c
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

#include "sdljump.h"
#include "setup.h"
#include "surface.h"
#include "sprite.h"
#include "tools.h"

extern SDL_Surface *screen, *realscreen;
extern L_gblOptions gblOps;

void initGblOps(void)
{   
    int i;
    gblOps.w = 0;
    gblOps.h = 0;
    gblOps.bpp = 32;
    
    gblOps.fps = 0;
    gblOps.rotMode = ROTFULL;
    gblOps.scrollMode = SOFTSCROLL;
    gblOps.mpLives = 3;

	/* Setting up the Default skin as skin*/
#ifdef WIN32
	gblOps.dataDir = malloc(sizeof(char)* (strlen("skins/")+strlen(DEFOLDER)+1));
    sprintf(gblOps.dataDir, "skins/%s",DEFOLDER);
#else
  #ifndef DEVEL
    gblOps.dataDir = malloc(sizeof(char)* (
		strlen(DATA_PREFIX) + strlen(PACKAGE) + strlen("/skins/") + strlen(DEFOLDER)+2));
    sprintf(gblOps.dataDir, "%s/%s/skins/%s",DATA_PREFIX,PACKAGE,DEFOLDER);
  #else
	gblOps.dataDir = malloc(sizeof(char) *(strlen("../skins/") + strlen(DEFOLDER)+1));
    sprintf(gblOps.dataDir, "../skins/%s",DEFOLDER);
  #endif
#endif
	
    /* Setting up the list of skin folders */
    gblOps.ntfolders = 1;
	gblOps.themeDirs = malloc(sizeof(char*)*gblOps.ntfolders);
	
#ifdef WIN32
	gblOps.themeDirs[0] = malloc(sizeof(char) * sizeof("skins"));
	strcpy(gblOps.themeDirs[0],"skins");
#else
  #ifndef DEVEL
	gblOps.themeDirs[0] = malloc(sizeof(char) *
				(strlen(DATA_PREFIX) + strlen("/skins") + strlen(PACKAGE) +2));
	sprintf(gblOps.themeDirs[0],"%s/%s/skins",DATA_PREFIX,PACKAGE);
  #else
    gblOps.themeDirs[0] = malloc(sizeof(char) * (strlen("../skins") +2));
	sprintf(gblOps.themeDirs[0],"../skins");
  #endif
#endif
    
    /* Default Keys */
    gblOps.keys[0][LEFTK] = KEY_LEFT1;
    gblOps.keys[1][LEFTK] = KEY_LEFT2;
    gblOps.keys[2][LEFTK] = KEY_LEFT3;
    gblOps.keys[3][LEFTK] = KEY_LEFT4;
    
    gblOps.keys[0][RIGHTK] = KEY_RIGHT1;
    gblOps.keys[1][RIGHTK] = KEY_RIGHT2;
    gblOps.keys[2][RIGHTK] = KEY_RIGHT3;
    gblOps.keys[3][RIGHTK] = KEY_RIGHT4;
    
    gblOps.keys[0][JUMPK] = KEY_UP1;
    gblOps.keys[1][JUMPK] = KEY_UP2;
    gblOps.keys[2][JUMPK] = KEY_UP3;
    gblOps.keys[3][JUMPK] = KEY_UP4;
    
    /* Default player names */
    for (i=0; i<MAX_PLAYERS; i++) {
        gblOps.pname[i] = malloc(sizeof(char)*(strlen(PNAME)+1));
        sprintf(gblOps.pname[i],"%s%d",PNAME,i+1);
    }
	
}

void cleanGblOps(void)
{
    int i;
    free(gblOps.dataDir);
    for (i=0; i<MAX_PLAYERS; i++) {
        free(gblOps.pname[i]);
    }
	for (i=0; i<gblOps.ntfolders; i++) {
        free(gblOps.themeDirs[i]);
    }
	free(gblOps.themeDirs);
}

int loadConfigFile(char* fname)
{
    FILE* tfile;
    int i;
    char str[MAX_CHAR];
    
    if ((tfile = fopen(fname,"r"))==NULL) {
        printf("\nWARNING: Can't open config file (%s). I will create one later. ",fname);
        return FALSE;
    }
    getValue_str(tfile,"protocol_version",str,FALSE);
    if (strcmp(str,PROT_VERS)!=0) {
        fclose(tfile);
        printf("\nWARNING: Config file (%s) is not compatible with this version of sdljump. I will rewrite it later.",fname);
        return FALSE;
    }
    
    gblOps.fps = getValue_int(tfile,"fps_limit");
    gblOps.rotMode = getValue_int(tfile,"rotation_mode");
    gblOps.scrollMode = getValue_int(tfile,"scroll_mode");
    gblOps.mpLives = getValue_int(tfile,"multiplayer_lives");
    
    // int junk1 = getValue_int(tfile,"use_opengl");
    gblOps.bpp = getValue_int(tfile,"bpp");
    gblOps.w = getValue_int(tfile,"screen_width");
    gblOps.h = getValue_int(tfile,"screen_height");
    gblOps.fullsc = getValue_int(tfile,"fullscreen");
    
    
    // int junk2 = getValue_int(tfile,"texture_filter");
    gblOps.dataDir = getValue_charp(tfile,"default_skin");
	
	gblOps.ntfolders = getValue_int(tfile,"skin_folders");
	gblOps.themeDirs = malloc(sizeof(char*)*gblOps.ntfolders);
	for (i = 0; i< gblOps.ntfolders; i++) {
		gblOps.themeDirs[i] = getValue_charp(tfile,"skin_dir");
	}
    
    for (i=0; i<MAX_PLAYERS; i++) {
        getValue_str(tfile,"player_name",str, FALSE);
        gblOps.pname[i] = malloc(sizeof(char)*(strlen(str)+1));
        strcpy(gblOps.pname[i],str);
        
        gblOps.keys[i][LEFTK] = getValue_int(tfile,"key_left");
        gblOps.keys[i][RIGHTK] = getValue_int(tfile,"key_right");
        gblOps.keys[i][JUMPK] = getValue_int(tfile,"key_jump");
    }
    
    fclose(tfile);
    
    return TRUE;
}

int writeConfigFile(char* fname)
{
    FILE* tfile;
    int i;
    
    if ((tfile = fopen(fname,"w"))==NULL) {
        printf("\nWARNING: Can't open config file (%s). Make sure that it is not being used by another app.",fname);
		return FALSE;
    }
    
    putComment(tfile,"This file has been automatically generated by SDLjump");
    putLine(tfile);
    putValue_str(tfile,"protocol_version",PROT_VERS);
    
    putLine(tfile);
    putComment(tfile,"Game options");
    putValue_int(tfile,"fps_limit",gblOps.fps);
    putValue_int(tfile,"rotation_mode",gblOps.rotMode);
    putValue_int(tfile,"scroll_mode",gblOps.scrollMode);
    putValue_int(tfile,"multiplayer_lives", gblOps.mpLives);
    
    putLine(tfile);
    putComment(tfile,"Graphics options");
    putValue_int(tfile,"bpp",gblOps.bpp);
    putValue_int(tfile,"screen_width",gblOps.w);
    putValue_int(tfile,"screen_height",gblOps.h);
    putValue_int(tfile,"fullscreen",gblOps.fullsc);
    
    putLine(tfile);
    putComment(tfile,"Skin options");
    putValue_str(tfile,"default_skin",gblOps.dataDir);
    putValue_int(tfile,"skin_folders",gblOps.ntfolders);
	for (i = 0; i< gblOps.ntfolders; i++) {
		putValue_str(tfile,"skin_dir",gblOps.themeDirs[i]);
	}
	
    putLine(tfile);
    putComment(tfile,"Player options");
    for (i=0; i<MAX_PLAYERS; i++) {
        putValue_str(tfile,"player_name",gblOps.pname[i]);
        putValue_int(tfile,"key_left",gblOps.keys[i][LEFTK]);
        putValue_int(tfile,"key_right",gblOps.keys[i][RIGHTK]);
        putValue_int(tfile,"key_jump",gblOps.keys[i][JUMPK]);
        putLine(tfile);
    }
    
    fclose(tfile);
    
    return TRUE;
}

char* getThemeComment(char* fname)
{
	FILE* fh;
	char str[MAX_CHAR];
	char* ret = NULL;
	char* file = NULL;
	
	file = malloc(sizeof(char)*(strlen(fname)+strlen(THEMEFILE)+1));
	strcpy (file,fname); strcat(file,THEMEFILE);
	
	if ((fh = fopen(file,"r")) == NULL){
        printf("\nWARNING: Can open theme file (%s).", file);
		free(file);
        return NULL;
    }
	
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,THEME_VERS)!=0) {
        fclose(fh);
        printf("\nWARNING: Theme file (%s) is not of the correct format.", file);
		free(file);
        return NULL;
    }
	
	ret = getValue_charp(fh,"comment");
	fclose(fh);

	printf("\nSuccessfully loaded comment from: %s",file);
	free(file);
	
	return ret;
}

int loadGraphics(graphics_t* data, char* fname)
{
    FILE* fh;
    char str[MAX_CHAR];
    SDL_Surface* surf;
    int i, manims;
    Uint8 b,g,r;
	char* file = NULL;
	
	file = malloc(sizeof(char)*(strlen(fname)+strlen(THEMEFILE)+1));
	strcpy (file,fname); strcat(file,THEMEFILE);
	
	printf("\nLoading theme: %s",file);

	if ((fh = fopen(file,"r")) == NULL){
        printf("\nERROR: Can open theme file (%s).", file);
		free(file);
        return 0;
    }
	
	/* 
	 * Global options 
	 */
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,THEME_VERS)!=0) {
        fclose(fh);
        printf("\nERROR: Theme file (%s) is not of the correct format.", file);
		free(file);
        return 0;
    }
		
	skipValueStr(fh); /* Comment */
	
    gblOps.w = getValue_int(fh,"window_width");
    gblOps.h = getValue_int(fh,"window_height");
    
    EngineInit(0, gblOps.fullsc, gblOps.w,gblOps.h,gblOps.bpp);
	
	gblOps.rotMode = getValue_int(fh,"default_rotation");
	
	/* 
	 * Menu data 
	 */
    getValue_str(fh,"menu_bg",str,TRUE);
    data->menuBg = JPB_LoadImg( str, 0, 0, 0);
	
	getValue_str(fh,"menu_font",str,TRUE);
    surf = IMG_Load(str);    
    data->menufont = SFont_InitFont(str, surf, 1);
    SDL_FreeSurface(surf);
	
	getValue_str(fh,"tip_font",str,TRUE);
    surf = IMG_Load(str);    
    data->tipfont = SFont_InitFont(str, surf, 1);
    SDL_FreeSurface(surf);
	
	data->mAlign = getValue_int(fh,"menu_align");
    data->tAlign = getValue_int(fh,"tip_align");
	
	data->menuX = getValue_int(fh,"menu_x");
    data->menuY = getValue_int(fh,"menu_y");
	data->menuW = getValue_int(fh,"menu_width");
	data->menuH = getValue_int(fh,"menu_height");
	data->menuGap = getValue_int(fh,"menu_gap");

	data->tipX = getValue_int(fh,"tip_x");
    data->tipY = getValue_int(fh,"tip_y");
	data->tipW = getValue_int(fh,"tip_width");
	data->tipH = getValue_int(fh,"tip_height");
	
	r = getValue_int(fh,"hl_red");
    g = getValue_int(fh,"hl_green");
    b = getValue_int(fh,"hl_blue");
	data->hlalpha = getValue_int(fh,"hl_alpha");
    data->hlcolor = SDL_MapRGB(screen->format, r, g, b);
	
	/* 
	 * In-game data
	 */
    getValue_str(fh,"game_bg",str,TRUE);
    data->gameBg = JPB_LoadImg( str, 0, 0, 0);

	getValue_str(fh,"live_pic",str,TRUE);
    data->livePic = JPB_LoadImg( str, 1, 0, 0);
	data->liveAlign = getValue_int(fh,"live_align");
	
    getValue_str(fh,"score_font",str,TRUE);
    surf = IMG_Load(str);    
    data->scorefont = SFont_InitFont(str, surf, 1);
    SDL_FreeSurface(surf);
    
	getValue_str(fh,"game_font",str,TRUE);
    surf = IMG_Load(str);    
    data->textfont = SFont_InitFont(str, surf, 1);
    SDL_FreeSurface(surf);
	
	r = getValue_int(fh,"g_red");
    g = getValue_int(fh,"g_green");
    b = getValue_int(fh,"g_blue");
	data->galpha = getValue_int(fh,"g_alpha");
    data->gcolor = SDL_MapRGB(screen->format, r, g, b);
	
    data->gameX = getValue_int(fh,"game_x");
    data->gameY = getValue_int(fh,"game_y");
    
    for (i=0; i<MAX_PLAYERS; i++) {
        data->scoreX[i] = getValue_int(fh,"score_x");
        data->scoreY[i] = getValue_int(fh,"score_y");
    	data->livesX[i] = getValue_int(fh,"lives_x");
        data->livesY[i] = getValue_int(fh,"lives_y");
    }
    
    getValue_str(fh,"floor_left",str,TRUE);
    data->floorL = JPB_LoadImg( str, 1, 0, 0);
    getValue_str(fh,"floor_right",str,TRUE);
    data->floorR = JPB_LoadImg( str, 1, 0, 0);
    getValue_str(fh,"floor_center",str,TRUE);
    data->floorC = JPB_LoadImg( str, 1, 0, 0);
    
    for (i=0; i<MAX_PLAYERS; i++) {
        getValue_str(fh,"hero_stand_anim",str,TRUE);
        data->heroSprite[i][H_STAND] = loadSpriteDataRot(str, 2);
        getValue_str(fh,"hero_run_anim",str,TRUE);
        data->heroSprite[i][H_WALK] = loadSpriteDataRot(str, 2);
        getValue_str(fh,"hero_jump_anim",str,TRUE);
        data->heroSprite[i][H_JUMP] = loadSpriteDataRot(str, 2);
    }
    
	/*
	 * Translation messages
	 */
	data->txt[txt_name] = getValue_charp(fh, "txt_name");
	data->txt[txt_floor] = getValue_charp(fh, "txt_floor");
	data->txt[txt_mode] = getValue_charp(fh, "txt_mode");
	data->txt[txt_time] = getValue_charp(fh, "txt_time");
	data->txt[txt_hscnote] = getValue_charp(fh, "txt_hscnote");
	data->txt[txt_newhsc] = getValue_charp(fh, "txt_newhsc");
	data->txt[txt_gameover] = getValue_charp(fh, "txt_gameover");
	data->txt[txt_askquit] = getValue_charp(fh, "txt_askquit");
	data->txt[txt_pause] = getValue_charp(fh, "txt_pause");
	
	data->msg[msg_newgame] = getValue_charp(fh, "msg_newgame");
	data->msg[msg_options] = getValue_charp(fh, "msg_options");
	data->msg[msg_highscores] = getValue_charp(fh, "msg_highscores");
	data->msg[msg_themes] = getValue_charp(fh, "msg_themes");
	data->msg[msg_quit] = getValue_charp(fh, "msg_quit");
	data->msg[msg_back] = getValue_charp(fh, "msg_back");
	data->msg[msg_themefolders] = getValue_charp(fh, "msg_themefolders");
	data->msg[msg_choostheme] = getValue_charp(fh, "msg_choostheme");
	data->msg[msg_addthemefolder] = getValue_charp(fh, "msg_addthemefolder");
	data->msg[msg_deletefolder] = getValue_charp(fh, "msg_deletefolder");
	data->msg[msg_editfolder] = getValue_charp(fh, "msg_editfolder");
	data->msg[msg_1playergame] = getValue_charp(fh, "msg_1playergame");
	data->msg[msg_2playergame] = getValue_charp(fh, "msg_2playergame");
	data->msg[msg_3playergame] = getValue_charp(fh, "msg_3playergame");
	data->msg[msg_4playergame] = getValue_charp(fh, "msg_4playergame");
	data->msg[msg_gameoptions] = getValue_charp(fh, "msg_gameoptions");
	data->msg[msg_graphicoptions] = getValue_charp(fh, "msg_graphicoptions");
	data->msg[msg_playernames] = getValue_charp(fh, "msg_playernames");
	data->msg[msg_redefinekeys] = getValue_charp(fh, "msg_redefinekeys");
	data->msg[msg_fpslimit] = getValue_charp(fh, "msg_fpslimit");
	data->msg[msg_jumpingrot] = getValue_charp(fh, "msg_jumpingrot");
	data->msg[msg_scrollmode] = getValue_charp(fh, "msg_scrollmode");
	data->msg[msg_mplives] = getValue_charp(fh, "msg_mplives");
	data->msg[msg_40fps] = getValue_charp(fh, "msg_40fps");
	data->msg[msg_100fps] = getValue_charp(fh, "msg_100fps");
	data->msg[msg_300fps] = getValue_charp(fh, "msg_300fps");
	data->msg[msg_nolimit] = getValue_charp(fh, "msg_nolimit");
	data->msg[msg_norot] = getValue_charp(fh, "msg_norot");
	data->msg[msg_orginalrot] = getValue_charp(fh, "msg_orginalrot");
	data->msg[msg_fullrot] = getValue_charp(fh, "msg_fullrot");
	data->msg[msg_fullrotaa] = getValue_charp(fh, "msg_fullrotaa");
	data->msg[msg_softscroll] = getValue_charp(fh, "msg_softscroll");
	data->msg[msg_hardscroll] = getValue_charp(fh, "msg_hardscroll");
	data->msg[msg_opengl] = getValue_charp(fh, "msg_opengl");
	data->msg[msg_bpp] = getValue_charp(fh, "msg_bpp");
	data->msg[msg_fullscreen] = getValue_charp(fh, "msg_fullscreen");
	data->msg[msg_8bpp] = getValue_charp(fh, "msg_8bpp");
	data->msg[msg_16bpp] = getValue_charp(fh, "msg_16bpp");
	data->msg[msg_24bpp] = getValue_charp(fh, "msg_24bpp");
	data->msg[msg_32bpp] = getValue_charp(fh, "msg_32bpp");
	data->msg[msg_yes] = getValue_charp(fh, "msg_yes");
	data->msg[msg_no] = getValue_charp(fh, "msg_no");
	data->msg[msg_player1name] = getValue_charp(fh, "msg_player1name");
	data->msg[msg_player2name] = getValue_charp(fh, "msg_player2name");
	data->msg[msg_player3name] = getValue_charp(fh, "msg_player3name");
	data->msg[msg_player4name] = getValue_charp(fh, "msg_player4name");
	data->msg[msg_player1keys] = getValue_charp(fh, "msg_player1keys");
	data->msg[msg_player2keys] = getValue_charp(fh, "msg_player2keys");
	data->msg[msg_player3keys] = getValue_charp(fh, "msg_player3keys");
	data->msg[msg_player4keys] = getValue_charp(fh, "msg_player4keys");
	data->msg[msg_redefkeyleft] = getValue_charp(fh, "msg_redefkeyleft");
	data->msg[msg_redefkeyright] = getValue_charp(fh, "msg_redefkeyright");
	data->msg[msg_redefkeyup] = getValue_charp(fh, "msg_redefkeyup");

	data->tip[tip_newgame] = getValue_charp(fh, "tip_newgame");
	data->tip[tip_options] = getValue_charp(fh, "tip_options");
	data->tip[tip_highscores] = getValue_charp(fh, "tip_highscores");
	data->tip[tip_themes] = getValue_charp(fh, "tip_themes");
	data->tip[tip_quit] = getValue_charp(fh, "tip_quit");
	data->tip[tip_back] = getValue_charp(fh, "tip_back");
	data->tip[tip_themefolders] = getValue_charp(fh, "tip_themefolders");
	data->tip[tip_choostheme] = getValue_charp(fh, "tip_choostheme");
	data->tip[tip_addthemefolder] = getValue_charp(fh, "tip_addthemefolder");
	data->tip[tip_themefolder] = getValue_charp(fh, "tip_themefolder");
	data->tip[tip_deletefolder] = getValue_charp(fh, "tip_deletefolder");
	data->tip[tip_editfolder] = getValue_charp(fh, "tip_editfolder");
	data->tip[tip_writefolder] = getValue_charp(fh, "tip_writefolder");
	data->tip[tip_1playergame] = getValue_charp(fh, "tip_1playergame");
	data->tip[tip_2playergame] = getValue_charp(fh, "tip_2playergame");
	data->tip[tip_3playergame] = getValue_charp(fh, "tip_3playergame");
	data->tip[tip_4playergame] = getValue_charp(fh, "tip_4playergame");
	data->tip[tip_gameoptions] = getValue_charp(fh, "tip_gameoptions");
	data->tip[tip_graphicoptions] = getValue_charp(fh, "tip_graphicoptions");
	data->tip[tip_playernames] = getValue_charp(fh, "tip_playernames");
	data->tip[tip_redefinekeys] = getValue_charp(fh, "tip_redefinekeys");
	data->tip[tip_fpslimit] = getValue_charp(fh, "tip_fpslimit");
	data->tip[tip_jumpingrot] = getValue_charp(fh, "tip_jumpingrot");
	data->tip[tip_scrollmode] = getValue_charp(fh, "tip_scrollmode");
	data->tip[tip_mplives] = getValue_charp(fh, "tip_mplives");
	data->tip[tip_40fps] = getValue_charp(fh, "tip_40fps");
	data->tip[tip_100fps] = getValue_charp(fh, "tip_100fps");
	data->tip[tip_300fps] = getValue_charp(fh, "tip_300fps");
	data->tip[tip_nolimit] = getValue_charp(fh, "tip_nolimit");
	data->tip[tip_norot] = getValue_charp(fh, "tip_norot");
	data->tip[tip_orginalrot] = getValue_charp(fh, "tip_orginalrot");
	data->tip[tip_fullrot] = getValue_charp(fh, "tip_fullrot");
	data->tip[tip_fullrotaa] = getValue_charp(fh, "tip_fullrotaa");
	data->tip[tip_softscroll] = getValue_charp(fh, "tip_softscroll");
	data->tip[tip_hardscroll] = getValue_charp(fh, "tip_hardscroll");
	data->tip[tip_opengl] = getValue_charp(fh, "tip_opengl");
	data->tip[tip_bpp] = getValue_charp(fh, "tip_bpp");
	data->tip[tip_fullscreen] = getValue_charp(fh, "tip_fullscreen");
	data->tip[tip_8bpp] = getValue_charp(fh, "tip_8bpp");
	data->tip[tip_16bpp] = getValue_charp(fh, "tip_16bpp");
	data->tip[tip_24bpp] = getValue_charp(fh, "tip_24bpp");
	data->tip[tip_32bpp] = getValue_charp(fh, "tip_32bpp");
	data->tip[tip_yes] = getValue_charp(fh, "tip_yes");
	data->tip[tip_no] = getValue_charp(fh, "tip_no");
	data->tip[tip_redefkeyleft] = getValue_charp(fh, "tip_redefkeyleft");
	data->tip[tip_redefkeyright] = getValue_charp(fh, "tip_redefkeyright");
	data->tip[tip_redefkeyup] = getValue_charp(fh, "tip_redefkeyup");
	data->tip[tip_inputname] = getValue_charp(fh, "tip_inputname");
	
    fclose(fh);
	
	free(file);
	
    return TRUE;
}

void freeGraphics(graphics_t* data)
{
    int i,j;
    for (i=0; i<MAX_PLAYERS; i++) {
    	for (j=0; j<HEROANIMS; j++)
        	freeSpriteDataRot(data->heroSprite[i][j]);
    }
    JPB_FreeSurface(data->gameBg);
    JPB_FreeSurface(data->livePic);
    JPB_FreeSurface(data->floorR);
    JPB_FreeSurface(data->floorL);
    JPB_FreeSurface(data->floorC);
    JPB_FreeSurface(data->menuBg);
    SFont_FreeFont(data->scorefont);
    SFont_FreeFont(data->menufont);
	SFont_FreeFont(data->tipfont);
	for (i = 0; i<MSG_COUNT; i++) {
		free(data->msg[i]);
	}
	for (i = 0; i<TIP_COUNT; i++) {
		free(data->tip[i]);
	}
	for (i = 0; i<TXT_COUNT; i++) {
		free(data->txt[i]);
	}
}

void resetEngine(graphics_t* gfxdata)
{
    freeGraphics(gfxdata);
    SDL_Quit();
    loadGraphics(gfxdata, gblOps.dataDir);
}

void EngineInit(int mouse, int fullscreen, int w, int h, int bpp)
{
	const SDL_VideoInfo* info = NULL;
	
    //Everything starts!!
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) <0 ) {
        printf("ERROR: SDL_Init did not work because: %s\n", SDL_GetError());
    }
    
    atexit(SDL_Quit); //this avoids exiting without ending SDL
    
    info = SDL_GetVideoInfo( );
    bpp = info->vfmt->BitsPerPixel;
    
	SetVideoSw(w, h, fullscreen, bpp);
	SDL_WM_SetCaption("SDLjump " VERSION " (Sofware rendering)",NULL);
    
    printf("\nVideo system initialized. BPP: %d, Resolution: %dx%d",
		   screen->format->BitsPerPixel,
		   screen->w,screen->h
		);
    
    if (mouse == 0) { //Disables the mouse
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void SetVideoSw(int w, int h, int fullscreen,int bpp)
{
    //Screen starts...
    if (fullscreen == 1) {//Fullscreen ON
        realscreen = SDL_SetVideoMode(w,h,bpp, 
									  SDL_ANYFORMAT |
									  SDL_FULLSCREEN |
									  SDL_HWSURFACE |
									  SDL_DOUBLEBUF);
		if (realscreen == NULL) {
            printf("\nERROR: The screen wasn't initialized because: %s", SDL_GetError());
        }
    }
	else { //Fullscreen OFF
        realscreen = SDL_SetVideoMode(w,h,bpp,SDL_HWSURFACE | SDL_DOUBLEBUF);
        if (realscreen == NULL) {
            printf("\nERROR: The screen wasn't initialized because: %s", SDL_GetError());
        }
    }

	if (!(realscreen->flags & SDL_DOUBLEBUF))
	{
		printf("\nCreating our own double buffer\n");

		screen = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_HWPALETTE,
									  realscreen->w, realscreen->h,
									  realscreen->format->BitsPerPixel,
									  realscreen->format->Rmask,
									  realscreen->format->Gmask,
									  realscreen->format->Bmask,
									  0);
		if (screen == NULL) {
            printf("\nERROR: The screen buffer wasn't initialized because: %s", SDL_GetError());
		}		
	}
	else {
		screen = realscreen;
	}
}
