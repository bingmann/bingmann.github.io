/*
 * $Id$
 */
#include <idl4glue.h>
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <new>
#include <assert.h>
#include <l4/thread.h>
#include <l4/schedule.h>
#include <l4/ipc.h>
#include <string.h>
#include <sdi/panic.h>
#include <sdi/log.h>
#include <sdi/locator.h>

#include <if/iflocator.h>
#include <if/iffile.h>
#include <if/ifsyscall.h>


#include <sdi/panic.h>

namespace
{
	static L4_ThreadId_t consoleid = L4_nilthread;
	static CORBA_Environment env (idl4_default_environment);
	static objectid_t consolehandle;
	static char* consoleNr = NULL;
}

// array of the german keyboard, where the position in the array
// corresponds with the scandode and kbgerman[scancode] = ascii
char kbgerman[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', 0, 0, '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'z', 'u', 'i', 'o', 'p', 0, '+', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0,	/* 39 */
   0, '^',   0,		/* Left shift */
 '#', 'y', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '-',   0,				/* Right shift */
    0,
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    127,	/* Delete Key */
    0,   0,   '<',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

// array of the german keyboard, where the position in the array
// corresponds with the scandode and kbgerman[scancode] = ascii
// with SHIFT pressed
char kbgerman_shift[128] =
{
    0,  27, '!', '"', '§', '$', '%', '&', '/', '(',	/* 9 */
  ')', '=', '?', 0, '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Z', 'U', 'I', 'O', 'P', 0, '*', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0,	/* 39 */
   0, '°',   0,		/* Left shift */
 '\'', 'Y', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', ';', ':', '_',   0,				/* Right shift */
    0,
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    127,	/* Delete Key */
    0,   0,   '>',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	

int CTRLpressed(uint32_t keyStatus) {
	return (keyStatus & 0x00000001) || (keyStatus & 0x00000002);
}

int ALTpressed(uint32_t keyStatus) {
	return (keyStatus & 0x00000004) || (keyStatus & 0x00000008);
}

int SHIFTpressed(uint32_t keyStatus) {
	return (keyStatus & 0x00000010) || (keyStatus & 0x00000020);
}

extern "C" {

	/**
	 * Translates a scancode to it's corresponding char representation,
	 * or 0 if there's no printable char. Except:
	 * ESC = 27, BACKSPACE = \b = 8, RETURN = \n = 13, DEL = 127
	 */
	char ScancodeToAscii(uint8_t scancode, uint32_t keyStatus) {
		if(SHIFTpressed(keyStatus)) {
			return kbgerman_shift[scancode];	
		} else {
			return kbgerman[scancode];
		}
	}
	
	L4_Bool_t isPrintable(uint8_t scancode, uint32_t keyStatus) {
		uint8_t ascii = ScancodeToAscii(scancode, keyStatus);
		if( 32 <= ascii && ascii <= 126) {
			return 1;	
		} else {
			return 0;
		} 
	}
	


	/**
	 * Writes to the console like printf.
	 */
	void writeConsole(const char* message, ...)
	{
		// get consoleid & handle, if not already acquired
		if(consoleid == L4_nilthread) {
			if(consoleNr == NULL && getenv("STDOUT") != NULL) {
				consoleNr = getenv("STDOUT");	
				LogMessage("Using env!");
			} else {
				consoleNr = "/console0";
			}
			LogMessage("Waiting for console");
			while(GetObject(consoleNr, IF_FILE_ID, &consoleid, &consolehandle) != OK) {
				L4_Yield();
				printf(".");
			}
		}
	
		char buf[160];

		va_list ap;
		va_start(ap, message);
		vsnprintf(buf, sizeof(buf), message, ap);
		va_end(ap);

		idlsize_t byteswritten;
		buffer_t msgbuf;
		msgbuf._buffer = new char[160];
		strcpy(msgbuf._buffer, buf);
		msgbuf._length = strlen(msgbuf._buffer);
		IF_FILE_Write((CORBA_Object)consoleid, consolehandle, 0, &byteswritten, &msgbuf, &env);	
		delete msgbuf._buffer;
				
	}

	/*
	 * Reads # readsize scancodes from the input buffer of the console
	 * and saves it into the buffer.
	 */	
	void readConsole(const idlsize_t readsize, buffer_t *buffer) {
		// get consoleid & handle, if not already acquired
		if(consoleid == L4_nilthread) {
			if(consoleNr == NULL && getenv("STDIN") != NULL) {
				consoleNr = getenv("STDIN");	
				LogMessage("Using env!");
			} else {
				consoleNr = "/console0";	
			}
			LogMessage("Waiting for console");
			while(GetObject(consoleNr, IF_FILE_ID, &consoleid, &consolehandle) != OK) {
				L4_Yield();
				LogMessage(".");
			}
		}
		
		IF_FILE_Read((CORBA_Object)consoleid, consolehandle, 0, readsize, buffer, &env);
		
	};


}
