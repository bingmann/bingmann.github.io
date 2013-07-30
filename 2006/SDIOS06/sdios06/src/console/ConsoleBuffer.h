#ifndef CONSOLEBUFFER_H_
#define CONSOLEBUFFER_H_

#endif /*CONSOLEBUFFER_H_*/

#include <idl4glue.h>
#include "libc/stdint.h"
#include <config.h>
#include <if/types.h>
#include <stdio.h>
#include <stdlib.h>


class ConsoleBuffer {
	
	char* buffer;				// 25 lins * 160 byte = at least a videobuffer of 4000
	unsigned int bufferSize;
	unsigned int screenStart;	// first position of the current screen in the whole buffer
	unsigned int cursorPosition;	// current Position of the cursor, always at even positions in 
									// the buffer, cause at the odd positions is the color saved
	unsigned int savedCursorPos;	// the CursorPosition if it was saved via "ESC[s"								
	
	uint8_t colorCode;			/*  the color and appearance of the text on the screen, coded in one byte
								 * the higher half-byte codes the background, MSB is set for blinking
								 * the lower Half-byte codes the foreground, bit 3 is set for brightness
								 * the remaining 3 bit in each half stand for RED, GREEN, BLUE and can be
								 * combined.
								 * Standard is 0x07 = 0000 0111: calm black background, matt white foreground
								 */
	uint8_t cursorColor;
	char* videoMemStart;
	int* hasFocus;
	
	
public:
	ConsoleBuffer(unsigned int size, char* videoMemStart, int* focus);
	~ConsoleBuffer();
	
	void CursorUp(int n);
	void CursorDown(int n);
	void CursorRight(int n);
	void CursorLeft(int n);
	void SetCursor(int z, int s);
	void SetCursor(int pos);
	void SaveCursor();
	void LoadCursor();
	void ClearScreen();
	void ClearLine(uint8_t n);
	void DeleteLast();
	void Delete();
	void NewLine();
	void appendChar(char c);
	void appendString(buffer_t *string);
	void ScreenUp(int n);
	void ScreenDown(int n);
	void SetColor(uint8_t c);
	void update();
	void PageUp();
	void PageDown();
	void SetGraphic(int n);
	
	
};
