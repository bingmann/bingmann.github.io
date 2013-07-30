#include "ConsoleBuffer.h"
#include <new>
#include <assert.h>
#include <sdi/panic.h>
#include <string.h>

static const unsigned int VIDEO_MEMORY_SIZE = 4000;
static const unsigned int LINE_LENGTH = 160;

ConsoleBuffer::ConsoleBuffer(unsigned int size, char* vmStart, int* focus) {
	if(size < VIDEO_MEMORY_SIZE) {
		panic("ConsoleBuffer to small! Must be at least 4000 bytes = screen size");	
	}
	hasFocus = focus;
	buffer = new char[size];
	bufferSize = size;
	memset(buffer, 0, size);
	videoMemStart = vmStart;
	colorCode = 0x07;
	cursorColor  = 0xf8;
	screenStart = 0;
	cursorPosition = 0;
	savedCursorPos = 0;
};

ConsoleBuffer::~ConsoleBuffer() {};


void ConsoleBuffer::CursorUp(int n) {};
void ConsoleBuffer::CursorDown(int n) {};

/**
 *  Moves the cursor n position to the left. If it is
 *  already at the edge of the screen, it has no effect.
 */
void ConsoleBuffer::CursorLeft(int n) {
	assert(cursorPosition % 2 == 0);
	unsigned int newPosition = cursorPosition - n * 2;
	if(newPosition < screenStart && newPosition > 0) {	// out of screen
		// shift screen one line up
		screenStart -= LINE_LENGTH;
			// reset color at the current position of the cursor
	}
	SetColor(colorCode);	// set the color of our last position back to normal
	cursorPosition = newPosition;	// go to the next ascii field
	SetColor(cursorColor);	// set the cursorColor	
	//update();	
};

/**
 *  Moves the cursor n position to the right. If it is
 *  already at the edge of the screen, it has no effect.
 */
void ConsoleBuffer::CursorRight(int n) {
	// reset color at the current position of the cursor
	assert(cursorPosition % 2 == 0);
	SetColor(colorCode);	// set the color of our last position back to normal
	if(cursorPosition + (n*2) >= screenStart + VIDEO_MEMORY_SIZE) {	// out of current screen
		if(cursorPosition + (n*2) >= bufferSize) {		// out of current buffer length
			// shift enerything # lines up
			int lines = 1;
			memmove(buffer, buffer + lines * 160, bufferSize - lines * 160);
			memset(buffer + bufferSize - lines * 160, 0, lines * 160); 
			cursorPosition -= LINE_LENGTH;
		}
		screenStart += LINE_LENGTH;
	}
	cursorPosition += (n*2);	// go to the next ascii field
	SetColor(cursorColor);	// set the cursorColor
	//update();
};

/*
 * Sets the cursor at the row z, column s
 */ 
void ConsoleBuffer::SetCursor(int z, int s) {
	
};

/*
 * Saves the current position of the cursor.
 */
void ConsoleBuffer::SaveCursor() {
	savedCursorPos = cursorPosition;
};

/*
 * Loads the saved cursor position, 0 as default.
 */
void ConsoleBuffer::LoadCursor() {
	SetColor(colorCode);
	cursorPosition = savedCursorPos;
	SetColor(cursorColor);
	update();
};

/*
 * Clears the screen and sets the cursor at the upper left corner.
 * TODO: reset with scrollbuffer
 */
void ConsoleBuffer::ClearScreen() {
	memset(buffer + screenStart, 0, VIDEO_MEMORY_SIZE);
	// setCursor at beginning of screenframe in buffer
	cursorPosition = screenStart;
	SetColor(cursorColor);
	update();
};

void ConsoleBuffer::ClearLine(uint8_t n) {
	if(0 == n) {
		// clear from cursorPosition till end of line
		memset(buffer + cursorPosition, 0, LINE_LENGTH - (cursorPosition % LINE_LENGTH));
		
	} else if (1 == n) {
		// clear from the beginning of the line till cursorPosition
		memset(buffer + cursorPosition - (cursorPosition % LINE_LENGTH), 0, (cursorPosition % LINE_LENGTH) + 1);
		
	} else if(2 == n) {
		// clear complete line
		memset(buffer + cursorPosition - (cursorPosition % LINE_LENGTH), 0, LINE_LENGTH);
	}
	SetColor(cursorColor);
	update();
};

/**
 * Sets the color c at the position of the cursor.
 * Attention: the position refers to the cursor position and
 * the color is saved in the next byte, after the ascci. 
 */
void ConsoleBuffer::SetColor(uint8_t c) {
	assert(cursorPosition % 2 == 0);
	buffer[cursorPosition + 1] = c;	
};

/*
 * Delets the last entered char, the one left of the cursor.
 * After that the cursor is set one step left.
 * Do nothing if already at the beginnin.
 */
void ConsoleBuffer::DeleteLast() {
	assert(cursorPosition % 2 == 0);
	if(cursorPosition > 0) {
		if(0 == cursorPosition % LINE_LENGTH) {	// at the beginning of a new line
			CursorLeft(1);
			// go to the last entered char
			while((cursorPosition > 0) && (0 == buffer[cursorPosition])) {
				if(0 == cursorPosition % LINE_LENGTH) {	// at the beginning of a the next new line stop
					update();
					return;
				}
				CursorLeft(1);
			}
		} else {
			CursorLeft(1);
		}
		buffer[cursorPosition] = 0;
		update();
	}
};

/*
 * Deletes the ascii at the current cursor position.
 */ 
void ConsoleBuffer::Delete() {
	assert(cursorPosition % 2 == 0);
	buffer[cursorPosition] = 0;
	update();
};

/**
 * Sets the cursor at the beginning of a new line.
 */
void ConsoleBuffer::NewLine() {
	assert(cursorPosition % 2 == 0);
	// reset color at current positioin
	SetColor(colorCode);
	buffer[cursorPosition] = ' ';	// this space character stands as representative for a '\n', but is not visible
	
	// set cursor at the beginning of a new line
	unsigned int newPosition = cursorPosition - (cursorPosition % LINE_LENGTH) + LINE_LENGTH;
	
	// check boundaries
	if(newPosition >= screenStart + VIDEO_MEMORY_SIZE) {	// out of screen
		if(newPosition >= bufferSize) {		// out of current buffer length
			// shift enerything # lines up
			int lines = 1;
			memmove(buffer, buffer + lines * 160, bufferSize - lines * 160);
			memset(buffer + bufferSize - lines * 160, 0, lines * 160); 
			newPosition -= lines * LINE_LENGTH;
			screenStart -= lines * LINE_LENGTH;
		}
		// shift screenStart one line ahead
		screenStart += LINE_LENGTH;
	}
	cursorPosition = newPosition;
	// set cursor color
	SetColor(cursorColor);
	update();
};

/*
 * Writes the char c at the position of the cursor, sets the current
 * color and moves the cursor forward.
 */
void ConsoleBuffer::appendChar(char c) {
	//TODO; check boundaries 
	if(cursorPosition >= screenStart + VIDEO_MEMORY_SIZE) {
		screenStart = cursorPosition - (cursorPosition % LINE_LENGTH) - 25 * LINE_LENGTH;
	}
	buffer[cursorPosition] = c;
	CursorRight(1);
};

/*
 *  Writes the content of the buffer string to the consoleBuffer
 */
void ConsoleBuffer::appendString(buffer_t *string) {
	for(unsigned int i=0; i<string->_length; i++) {
		appendChar(string->_buffer[i]);
	}
};

/*
 * Copies the current part of the console buffer to the video memory
 * if this console has the focus.
 */	
void ConsoleBuffer::update() {
	if(*hasFocus != 0) {
		memcpy(videoMemStart, buffer + screenStart, VIDEO_MEMORY_SIZE);	
	}
};

/*
 * Moves the scrollbuffer one line down.
 */ 
void ConsoleBuffer::PageDown() {
	if(screenStart + VIDEO_MEMORY_SIZE + LINE_LENGTH <= bufferSize) {
		screenStart += 160;
		update();	
	}
};

/*
 * Moves the scrollbuffer one line up.
 */
void ConsoleBuffer::PageUp() {
	if(screenStart >= 160) {
		screenStart -= 160;
		update();	
	}
};

/*
 * Set the graphics parameters. Supports:
 * n = ...
 * 0	All attributes off	0000 0000
 * 1	Bright				xxxx 1xxx
 * 5 	Blink				1xxx xxx0
 * 30	Black foreground	xxxx x000
 * 31	Red foreground		xxxx x100
 * 32	Green foreground	xxxx x010
 * 33	light blue foreground	xxxx x011
 * 34	Blue foreground		xxxx x001
 * 35	Magenta foreground	xxxx x101
 * 36	Cyan foreground		xxxx x110
 * 37	White foreground	xxxx x111
 * 40	Blach background	x000 xxxx
 * 41	Red backgroundd		x100 xxxx
 * 42	Green background	x010 xxxx
 * 43	Yellow background	x011 xxxx
 * 44	Blue background		x001 xxxx
 * 45	Magenta background	x101 xxxx
 * 46	Cyan background		x110 xxxx
 * 47	White background 	x111 xxxx
 * 
 */
void ConsoleBuffer::SetGraphic(int n) {
	switch (n) {
		case 0:
			colorCode = 0x07;	// reset all
			break;			
		case 1:
			colorCode |=  0x8;	// bright
			break;		
		case 5:
			colorCode |= 0x80;	// blink
			break;	
		case 30:
			colorCode &= 0xf8;	// Black foreground	xxxx x000
			break;		
		case 31:
			colorCode &= 0xf8;	// Red foreground xxxx x100
			colorCode |= 0x04;
			break;
		case 32:
			colorCode &= 0xf8;	//	Green foreground xxxx x010
			colorCode |= 0x02;
			break;
		case 33:
			colorCode &= 0xf8;	// light blue foreground xxxx x011
			colorCode |= 0x03;
			break;
		case 34:
			colorCode &= 0xf8;	// Blue foreground xxxx x001
			colorCode |= 0x01;
			break;
		case 35:
			colorCode &= 0xf8;	// Magenta foreground xxxx x101
			colorCode |= 0x05;
			break;
		case 36:
			colorCode &= 0xf8;	// Cyan foreground xxxx x110
			colorCode |= 0x06;
			break;
		case 37:
			colorCode &= 0xf8;	// 37	White foreground	xxxx x111
			colorCode |= 0x07;
			break;
		case 40:
			colorCode &= 0x8f;	// Blach background	x000 xxxx
			break;
		case 41:				// Red backgroundd		x100 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x40;
			break;
		case 42:	// Green background	x010 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x20;
			break;
		case 43:	// light blue background	x011 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x30;
			break;
		case 44:	// Blue background		x001 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x10;
			break;
		case 45:	// Magenta background	x101 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x50;
			break;
		case 46:	// Cyan background		x110 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x60;
			break;
		case 47:	// White background 	x111 xxxx
			colorCode &= 0x8f;
			colorCode |= 0x70;
			break;
	}
};

