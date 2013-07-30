#include "VirtualConsole.h"

#include <idl4glue.h>
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <assert.h>
#include <string.h>
#include <l4/thread.h>
#include <l4/ipc.h>
#include <l4/kip.h>

#include <sdi/panic.h>
#include <sdi/log.h>
#include <sdi/locator.h>

#include <if/iflocator.h>
#include <if/iflogging.h>
#include <if/ifsyscall.h>


VirtualConsole::VirtualConsole(int consoleId, char* videoMemStart) {
	myConsoleId = consoleId;
	videoBuffer = videoMemStart;
	hasFocus = 0;
	inputBuffer = new uint8_t[INPUT_BUFFER_LENGTH];
	nextFreeIn = 0;
	keyStatus = 0;
	consoleBuffer = new ConsoleBuffer(CONSOLE_BUFFER_LENGTH, videoMemStart, &hasFocus);
	consoleBuffer->ClearScreen();
	buffer_t buf;
	buf._buffer = new char[128];
	sprintf(buf._buffer, "Hello to console%d", consoleId);
	buf._length = strlen(buf._buffer);
	consoleBuffer->appendString(&buf);
	consoleBuffer->NewLine();
};

VirtualConsole::~VirtualConsole() {
	
}
		
/*
 * Writes the content of buffer to the videoBuffer. The content of the buffer
 * should be ascii codes, special control codes are filterd out, like
 * ESC(27), BACKSPACE('\b'=8, DEL(127), RETURN('\n'=13), and
 * ANSI Escape Sequences to control the screen.
 */
idlsize_t VirtualConsole::write(const buffer_t *buffer, const uint32_t pos) {
	// parse the buffer for special control sequences, delete, backspace, return, ...

	//printf("LEN=%d\n", buffer->_length);
	//printf("BUF=%s\n", buffer->_buffer);
 
	for(CORBA_unsigned_long i=0; i<buffer->_length; i++) {
		char c = buffer->_buffer[i];
		//printf("%c", c);
		// check if printable char between 32 and 126
		if((32 <= c) && (c <= 126) ) {
			// than print it on the screen
			consoleBuffer->appendChar(c);
			
		// else it ougth to be a special, nonprintable ascii
		} else if('\b' == c)  {	// BACKSPACE
			consoleBuffer->DeleteLast();
							
		} else if('\n' == c) {	// RETURN
			consoleBuffer->NewLine();
			
		} else if(127 == c) {	// DELETE
			consoleBuffer->Delete();

		} else if(27 == c) {	// ANSI Escape Code?
			if(i+2 < buffer->_length) {
				if(buffer->_buffer[i+1] == '[') {	// next is a [, it's definitly an escape code
					// escape code
					if(buffer->_buffer[i+2] == 's') {
						consoleBuffer->SaveCursor();
						i += 2;
					} else if(buffer->_buffer[i+2] == 'u') {
						consoleBuffer->LoadCursor();
						i += 2;
					} else if(i+3 < buffer->_length && 48 <= buffer->_buffer[i+2] && buffer->_buffer[i+2] <= 57) {	// next is a number
						uint32_t n = 0;	// the accumulated number
						// get all the nummers and merge them to one 
						int k=i+2;
						// as long as element k is a number
						while(k < buffer->_length && 48 <= buffer->_buffer[k] && buffer->_buffer[k] <= 57) {
							n = n * 10 + (buffer->_buffer[k] - 48);	// ascii: 48 = 0, substract 48 to get from ascii to the value
							k++;
						}
						// if there is a next one, which determines the action
						if(k < buffer->_length) {
							if(buffer->_buffer[k] == 'm') {
								consoleBuffer->SetGraphic(n);
							} else 	if(buffer->_buffer[k] == 'C') {	// RIGHT
								consoleBuffer->CursorRight(n);
								consoleBuffer->update();
							} else if(buffer->_buffer[k] == 'D') {	// LEFT
								consoleBuffer->CursorLeft(n);
								consoleBuffer->update();
							} else if(buffer->_buffer[k] == 'K') {	// Clear (part) of the line
								consoleBuffer->ClearLine(n);
							} else if(buffer->_buffer[k] == 'J' && 2 == buffer->_buffer[k-1]) {	// Clear Screen
								consoleBuffer->ClearScreen();
								ClearInputBuffer();
							}
							// go ahead at the right position in the buffer
							i = k;
						}
					}
				}
			}
		}

	}

	consoleBuffer->update();
	return buffer->_length;
};

/*
 * Reads the scancodes from the inputBuffer.
 */
CORBA_unsigned_long VirtualConsole::read(idlsize_t readsize, CORBA_char* buf) {
	if(nextFreeIn == 0) {
		return 0;	// InputBuffer is empty	

	} else if(nextFreeIn <= readsize) {
		// less input available as buffersize
		memcpy(buf, inputBuffer, nextFreeIn);
		// all input consumed, reset nextFreeIn
		int filled = nextFreeIn;
		nextFreeIn = 0;
		return filled;
		
	} else {
		// more in input buffer, than buf can take. copy only the first part
		memcpy(buf, inputBuffer, readsize);
		// shift the second part to the start of the inputBuffer
		// memmove better???
		int size = nextFreeIn - readsize;
		memcpy(inputBuffer, inputBuffer + readsize, size);
		nextFreeIn = size;
		return readsize;
	}
};

/*
 * Adds a scancode from the keyboard to the inputBuffer of this console. The
 * scancode is not processed or otherwise translated.
 * TODO: Scan for ALT+F1 ... for console switch
 */
void VirtualConsole::addInput(uint8_t c) {
	if(nextFreeIn < INPUT_BUFFER_LENGTH) {
		inputBuffer[nextFreeIn] = c;
		nextFreeIn++;
		//LogMessage("Added %u to input buffer of console %d", c, myConsoleId);
	} else {
		LogMessage("InputBuffer full in console %d", myConsoleId);
	}
};

void VirtualConsole::ClearInputBuffer() {
	nextFreeIn = 0;	
}
