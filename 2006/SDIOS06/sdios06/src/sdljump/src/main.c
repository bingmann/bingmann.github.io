/*
 * SDLjump
 * (C) 2005 Juan Pedro Bol�ar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * main.c
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
#include "game.h"
#include "menu.h"
#include "records.h"

SDL_Surface * screen = NULL;
SDL_Surface * realscreen = NULL;

L_gblOptions gblOps;

void displayHelp();

void displayInfo();

int parseArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    graphics_t gfxdata;
	char* cfgFile;
	char* hscFile;
    char* homeDir;
    
    /* Get the config file name */
    #ifndef WIN32 
#ifdef CONFIG_ARCH_IA32
    homeDir = "/minixfs0/sdljump"; // getenv("HOME");
#else
	homeDir = getenv("HOME");
#endif
    cfgFile = malloc(sizeof(char)* (strlen(homeDir) + strlen(CFGFILE) +1) );
	hscFile = malloc(sizeof(char)* (strlen(homeDir) + strlen(HSCFILE) +1) );
	strcpy(cfgFile,homeDir);
    strcpy(hscFile,homeDir);
    #else
    cfgFile = malloc(sizeof(char)* strlen(CFGFILE) +1 );
	hscFile = malloc(sizeof(char)* strlen(HSCFILE) +1 );
	cfgFile[0] = hscFile[0] = '\0';  
    #endif
	strcat(cfgFile,CFGFILE);
    strcat(hscFile,HSCFILE);
	
	if (!loadConfigFile(cfgFile)) {
        /* Set default options */
        initGblOps();
    }
	if (!loadRecords(hscFile, gblOps.records)) {
		defaultRecords(gblOps.records);		
	}
    
    /* Parse args */
    if (parseArgs(argc, argv)) {
        return 1;
    }
    
	if (!loadGraphics(&gfxdata, gblOps.dataDir))
		return 1;

    mainMenu(&gfxdata);
    
    /* Free some things */
    freeGraphics(&gfxdata);
    
    // writeConfigFile(cfgFile);
    // writeRecords(hscFile, gblOps.records);
    
    cleanGblOps();
    
    printf("\n\nHave a nice day! \n\n");
        
    return 0;
}

void displayHelp()
{
    printf(
    "\n SDLjump, an xjump clone. By Juan Pedro Bolivar Puente.\n"
    " This software can be redistributed and modified under the terms of the GPL.\n\n"
    " usage: sdljump [THEME_DIR] [OPTIONS] \n"
    " availible options:\n"
    " -w <int>  --width <int>   Forces the screen width to <int> units.\n"
    " -h <int>  --height <int>  Forces the screen height to <int> units.\n"
    " -b <int>  --bpp <int>     Sets the screen bitdepth to <int>. \n"
    " -f        --fullscreen    Force fullscreen mode.\n"
    " -?        --help          Displays this help screen.\n"
    " \n Example: sdljump myTheme -o -f\n"
    );  
}

void displayInfo()
{
    printf(
    "\n*********************************************************************"
    "\n*                            SDLjump                                *"
    "\n*********************************************************************"
    "\nCopyright (C) 2005, Juan Pedro Bolivar Puente\n"
    );
    
    printf("\nVERSION: %s", VERSION);
    
    printf(
    "\n\n SDLjump is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or\n"
    "(at your option) any later version.\n\n"
    "SDLjump is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with SDLjump; if not, write to the Free Software\n"
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
    );
    printf("\n Data Folder: %s\n", gblOps.dataDir);
}

int parseArgs(int argc, char *argv[])
{
    int i;
    for (i=1; i<argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] != '-') { /* Short arg */
                switch (argv[i][1]) {
                    case 'w':
                        if ((i+1) < argc) {
                            gblOps.w = atoi(argv[i+1]);
                            i++;
                        } break;
                    case 'h':
                        if ((i+1) < argc) {
                            gblOps.h = atoi(argv[i+1]);
                            i++;
                        } break;
                    case 'b':
                        if ((i+1) < argc) {
                            gblOps.bpp = atoi(argv[i+1]);
                            i++;
                        } break;
                    case 'f':
                        gblOps.fullsc = TRUE; break;
                    case '?':
                        displayHelp();
                        return 1; break;
                }
            } else { /* Long arg */
                if (strcmp(argv[i],"--width") == 0) {
                    if ((i+1) < argc) {
                        gblOps.w = atoi(argv[i+1]);
                        i++;
                    } 
                }
                if (strcmp(argv[i],"--height") == 0) {
                    if ((i+1) < argc) {
                        gblOps.h = atoi(argv[i+1]);
                        i++;
                    } 
                }
                if (strcmp(argv[i],"--bpp") == 0) {
                    if ((i+1) < argc) {
                        gblOps.bpp = atoi(argv[i+1]);
                        i++;
                    } 
                }
                if (strcmp(argv[i],"--fullscreen") == 0) {
                        gblOps.fullsc = TRUE;
                }
                if (strcmp(argv[i],"--help") == 0) {
                    displayHelp();
                    return 1;
                }
            }
        } else { /* data folder */
            free(gblOps.dataDir);
            gblOps.dataDir = malloc((strlen(argv[i])+1)*sizeof(char));
            strcpy(gblOps.dataDir, argv[i]);
        }
    }
    
    return FALSE;
}
