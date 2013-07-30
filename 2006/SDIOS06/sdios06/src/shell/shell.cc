#include <new>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <sdi/log.h>
#include <sdi/locator.h>
#include <sdi/console.h>
#include <sdi/elfexec.h>

#include "shell.h"


void InsertChar( char* string, const size_t maxlength, const uint32_t pos, char c )
{
	if( string == NULL || pos >  strlen( string ) || strlen( string ) + 1 >= maxlength )
		return;
	
	char* sptr = string + strlen( string ); // Set the pointer to the end of the string
	
	sptr[1] = '\0'; // Insert a new trailing zero
	
	// Move the chars behind pos
	for( ; sptr >= string + pos; sptr-- )
	{
		sptr[1] = sptr[0];
	}
	
	// Set new char
	sptr[1] = c;
}


void RemoveChar( char* string, const uint32_t pos )
{
	if( string == NULL || pos > strlen( string ) )
		return;
	
	char* sptr = string + pos;

	for( ; sptr <= string + strlen( string ); sptr++ )
	{
		sptr[0] = sptr[1];
	}
}


void StrTail( const char* src, const uint32_t pos, char* dest, const size_t max )
{	
	if( src == NULL || dest == NULL || pos > strlen( src ) )
		return;
	
	src += pos;
	
	const char* maxaddr = src + max;
	
	for( ; *src != '\0' && src < maxaddr; src++ )
		*(dest++) = *src;

	*dest = '\0';
}


bool StrStartsWith( const char* str, const char* head )
{
	if( str == NULL || head == NULL )
		return( false );
	
	while( *head != '\0' )
	{
		if( *( str++ ) != *( head++ ) )
			return( false );
	}
	
	return( true );
}

char **dupenviron()
{
	int envc;

	for (envc = 0; environ[envc]; ++envc)
		;
	envc++;

	char **dupenv = (char**)malloc(sizeof(char*) * envc);
	memcpy(dupenv, environ, sizeof(char*) * envc);

	return dupenv;
}

int myputenv(const char *string, char ***myenviron)
{
	size_t len;
	int envc;
	int remove=0;
	char *tmp;
	const char **ep;

	if (!(tmp = strchr(string,'='))) {
		len = strlen(string);
		remove = 1;
	}
	else {
		len = tmp - string + 1;
	}

	for (envc = 0, ep = (const char**)*myenviron; (ep && *ep); ++ep)
	{
		if (*string == **ep && memcmp(string, *ep, len) == 0) {
			if (remove) {
				for (; ep[1]; ++ep) ep[0] = ep[1];
				ep[0] = NULL;
				return 0;
			}
			*ep = string;
			return 0;
		}
		++envc;
	}

	if (tmp)
	{
		char **newenv = (char**)realloc(*myenviron,
										(envc+2) * sizeof(char*));
		if (!newenv) return -1;

		memcpy(newenv, *myenviron, envc * sizeof(char*));

		newenv[envc] = (char*)string;
		newenv[envc+1] = 0;

		*myenviron = newenv;
	}

	return 0;
}

int mysetenv(const char *name, const char *value, int overwrite, char ***myenviron)
{
	if (getenv(name))
	{
		if (!overwrite) return 0;
		myputenv(name, myenviron);
	}

	{
		char *c = (char*)malloc(strlen(name) + strlen(value) + 2);
		strcpy(c, name);
		strcat(c, "=");
		strcat(c, value);
		return myputenv(c, myenviron);
	}
}

Shell::Shell()
{
	// prepare input buffer
	inbuf._buffer  = new CORBA_char[COMMAND_LENGTH];
	inbuf._length  = COMMAND_LENGTH;
	inbuf._maximum = COMMAND_LENGTH;
	
	keyStatus = 0;
	cursorpos = 0;

	terminate = false;
	
	// prepare command list
	current_command = 0;
	display_command = 0;
	memset( &command, 0, sizeof( command ) );
}


Shell::~Shell()
{
	// ...
}


void Shell::WritePrompt()
{
	char path[CWDPATH_LENGTH];
	getcwd( path, sizeof( path ) );
	writeConsole( "\n\33[1m%s\33[0m %c ", path, PROMPT_CHAR );
}


void Shell::DisplayToCurrent()
{
	strncpy( command[current_command],
	         command[display_command],
	         sizeof( command[current_command] ) );
	
	display_command = current_command;
	
	cursorpos = strlen( command[current_command] );
	
	writeConsole( "%c[%dD%c[0K%s", 27, cursorpos, 27, command[display_command] );
}


void Shell::ProcessInput( uint8_t scancode )
{
	if( scancode == 0xe0 )
	{
		// extended scancode, the next scancode is the real key
		// set extended flag
		keyStatus |= FLAG_EXTENDED;
		return;
	}
	else
	{
		// clear extended flag
		keyStatus &= ~( FLAG_EXTENDED );
		
		// I know that this is buggy, but I don't care ATM becaus I don't use it
	}
	
	switch( scancode )
	{
		case 0x38:
			// ALT pressed
			keyStatus |= FLAG_ALT;
			return;
		
		case 0xb8:
			// ALT released
			keyStatus &= ~( FLAG_ALT );
			return;
		
		case 0x1d:
			// CTRL pressed
			keyStatus |= FLAG_CTRL;
			return;
			
		case 0x9d:
			// CTRL released
			keyStatus &= ~( FLAG_CTRL );
			return;
		
		case 0x2a:
		case 0x36:
			// SHIFT pressed
			keyStatus |= FLAG_SHIFT;
			return;
			
		case 0xaa:
		case 0xb6:
			// SHIFT released
			keyStatus &= ~( FLAG_SHIFT );
			return;
			
		case 0x4b:
			// LEFTARROW pressed
			if( cursorpos > 0 )
			{
				writeConsole( "%c[1D", 27 );
				cursorpos--;
			}
			return;
			
		case 0x4d:
			// RIGHTARROW pressed
			if( cursorpos < strlen( command[current_command] ) )
			{
				writeConsole( "%c[1C", 27 );
				cursorpos++;
			}
			return;
		
		case 0x47:
			// HOME pressed
			writeConsole( "%c[%dD", 27, cursorpos );
			cursorpos = 0;
			return;
		
		case 0x4f:
			// END pressed
			writeConsole( "%c[%dC", 27, strlen( command[display_command] ) - cursorpos );
			cursorpos = strlen( command[display_command] );
			return;
		
		case 0x48:
		{
			// UPARROW pressed
			uint8_t new_display_command = ( display_command + NUM_COMMANDS - 1 ) % NUM_COMMANDS;
			if( new_display_command != current_command && command[new_display_command][0] != '\0' )
			{
				display_command = new_display_command;
				
				// clear the line and write the new command
				writeConsole( "%c[%dD%s%c[0K", 27, cursorpos, command[display_command], 27 );
				
				cursorpos = strlen( command[display_command] );
			}
			return;
		}
		
		case 0x50:
			// DOWNARROW pressed
			if( display_command != current_command )
			{
				display_command = ( display_command + 1 ) % NUM_COMMANDS;
				
				// clear the line and write the new command
				writeConsole( "%c[%dD%s%c[0K", 27, cursorpos, command[display_command], 27 );
				
				cursorpos = strlen( command[display_command] );
			}
			return;
			
		case 0x0e:
			// BACKSPACE pressed
			
			if( display_command != current_command )
				DisplayToCurrent();
			
			// remove char from command line
			if( cursorpos > 0 )
			{				            
				char tail[COMMAND_LENGTH];
				StrTail( command[current_command], cursorpos, tail, sizeof( tail ) );
				writeConsole( "\b%c[s%s %c[u", 27, tail, 27 );

				RemoveChar( command[current_command],
				            cursorpos - 1 );

				cursorpos--;
			}
			return;
			
		case 0x53:
			// DEL pressed
			
			return;
			
		case 0x1c:
			// RETURN pressed
			if( display_command != current_command )
				DisplayToCurrent();
			
			writeConsole( "%c[%dC", 27, strlen( command[current_command] ) - cursorpos );
			if( command[current_command][0] != '\0' )
			{
				writeConsole( "\n" );
				// writeConsole( command[current_command] );
				Execute();
				current_command = ( current_command + 1 ) % NUM_COMMANDS; // select next command buffer
				display_command = current_command;
			}

			memset( command[current_command], 0, sizeof( command[current_command] ) ); // clear the buffer
			cursorpos = 0; // reset cursorpos
			WritePrompt(); // prompt for next command
			return;
			
		default:
			if( scancode < 128 && isPrintable( scancode, keyStatus ) )
			{
				if( display_command != current_command )
					DisplayToCurrent();
				
				// convert scancode to ascii with console lib
				char c = ScancodeToAscii( scancode, keyStatus );
				
				// write char (and tail) to console
				char tail[COMMAND_LENGTH];
				StrTail( command[current_command], cursorpos, tail, sizeof( tail ) );
				writeConsole( "%c%c[s%s%c[u", c, 27, tail, 27 );

				// add char to the command line
				InsertChar( command[current_command],
				            sizeof( command[current_command] ),
				            cursorpos,
				            c );

				cursorpos++;
			}
	}	
}


void Shell::Execute()
{
	if( command[current_command][0] == '\0' )
		return;		
	
	/*
	 * This Part parses the command line and counts the args which are concatenated to
	 * a large string. argv[n] points to the n-th argument.
	 * (This parser is powered by Russisch Brot)
	 */
	uint8_t argc = 0;
	char** argv = (char**)alloca(sizeof(char*) * 256);
	
	char argstr[COMMAND_LENGTH];
	memset( argstr, 0, sizeof( argstr ) );
	
	argv[0] = argstr;
	
	char* cptr;
	char* asptr = argstr;
	
	for( cptr = command[current_command]; *cptr != '\0'; cptr++ )
	{
		if( *cptr == ' ' )
		{
			// ignore multiple spaces
			if( argv[argc] == asptr )
				continue;
			
			*( asptr++ ) = '\0';
			argv[++argc] = asptr;
		}
		else
			*( asptr++ ) = *cptr;
	}
	
	// Do we have a last arg that wasn't processed by the loop?
	if( argv[argc] != asptr )
	{
		*asptr = '\0';
		argc++;
	}

	argv[argc] = NULL;
	
	if( argc == 0 )
		return;

	// create an environment for the program
	char **progenviron = dupenviron();

	// first arguments containing a = are read as environment variables to the
	// program
	while(argc > 0 && strchr(argv[0], '=') != NULL)
	{

		 // special shortcut to set all console devices
		if (strncmp(argv[0], "VT=", 3) == 0 &&
			isdigit(argv[0][3]) && (argv[0][4] == 0) || (isdigit(argv[0][4]) && argv[0][5] == 0))
		{
			char consolenum[32];
			snprintf(consolenum, sizeof(consolenum), "/console%c%c", argv[0][3], argv[0][4]);

			mysetenv("STDIN", consolenum, true, &progenviron);
			mysetenv("STDOUT", consolenum, true, &progenviron);
			mysetenv("STDERR", consolenum, true, &progenviron);
		}
		else
		{
			myputenv(argv[0], &progenviron);
		}
		
		argc--;
		argv++;
	}
	
	if( strcmp( argv[0], "cd" ) == 0 )
	{
		if( argc == 1 )
		{
			// the root is our home directory
			chdir( "/" );
		}
		else
		{
			char newcwd[256];
			
			if( argv[1][0] == '/' )
			{
				// absolute path
				strncpy( newcwd, argv[1], sizeof( newcwd ) );
			}
			else
			{
				// relative path
				char oldcwd[256];				
				getcwd( oldcwd, sizeof( oldcwd ) );

				strncpy( newcwd, oldcwd, sizeof( newcwd ) );

				if( newcwd[strlen( newcwd ) - 1] != PATH_SEPARATOR )
					strcat( newcwd, "/" );
					
				strcat( newcwd, argv[1] );
			}

			if( GetObject( newcwd, IF_DIRECTORY_ID, NULL, NULL ) == OK )
				chdir( newcwd );
			else
				writeConsole( "cd: Directory not found.." );
		}
	}
	else if (strcmp(argv[0], "set") == 0)
	{
		for(unsigned int ei = 0; progenviron[ei]; ei++)
			writeConsole("%s\n", progenviron[ei]);
	}
	else if (strcmp(argv[0], "setenv") == 0)
	{
		for(unsigned int ai = 1; ai < argc; ai++)
		{
			// has to be a newly allocate string
			char *envstr = strdup(argv[ai]);
			putenv(envstr);

			writeConsole("setenv %s\n", envstr);
		}
	}
	else if (strcmp(argv[0], "exit") == 0)
	{
		terminate = true;
	}
	else
	{	
		static const char *defpath = "/minixfs0/bin/";

		if (*argv[0] != '/')
		{
			char *fullpath = (char*)alloca(strlen(defpath) + strlen(argv[0]) + 1);

			snprintf(fullpath, strlen(defpath) + strlen(argv[0]) + 1,
					 "%s%s", defpath, argv[0]);

			argv[0] = fullpath;
		}
		
		// dont WaitFor if the last argument is a "&" => background process
		bool startbackground = false;
		if (argc > 0 && strcmp(argv[argc-1], "&") == 0)
		{
			argc--;
			argv[argc] = NULL;

			startbackground = true;
		}

		L4_ThreadId_t child = sdi_elfexecfve(argv[0], argv, progenviron);

		if (L4_IsNilThread(child))
		{
			writeConsole("yash: Could not run '%s'.\n", argv[0]);
		}
		else if (!startbackground)
		{
			L4_Word_t retcode;	

			if (!sdi_waitforever(child, &retcode)) {
				writeConsole("yash: error in WaitForever\n");
			}

			writeConsole("yash: '%s' terminated with '%lu'.", argv[0], retcode);
		}
		else {
			writeConsole("yash: '%s' started in background.\n", argv[0]);
		}
	}

	free(progenviron);
}

void Shell::Run()
{
	WritePrompt();
	
	while( !terminate )
	{
		// read scancode from console-server
		readConsole( COMMAND_LENGTH, &inbuf );

		if( inbuf._length > 0 )
		{
			uint8_t scancode;
			
			for( uint32_t i=0; i < inbuf._length; i++ )
			{
				scancode = inbuf._buffer[i];
				ProcessInput( scancode );
			}
		}	
	}
}
