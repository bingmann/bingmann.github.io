#include <idl4glue.h>
#include "libc/stdint.h"
#include <config.h>
#include <if/types.h>
#include "ConsoleBuffer.h"
#include <stdio.h>
#include <stdlib.h>


// length of the buffer, where the entered scancodes are saved until they're read
const unsigned int INPUT_BUFFER_LENGTH = 256;		
const unsigned int VIDEO_BUFFER_LENGTH = 4000;
const unsigned int CONSOLE_BUFFER_LENGTH = 8000;


/**
 * Representing a virtual Console.
 */
class VirtualConsole {


	uint8_t* inputBuffer;		// the Buffer where entered scancodes are stored until they're read	
	char* videoBuffer;			// saves the Input to the console from "write", shown on the screen
	int myConsoleId;			// each console as a id: console 1, 2, ...
	unsigned int nextFreeIn;	// the current position in the inputBuffer for the next element

	uint32_t keyStatus;			/* saves the currently hold modifier keys like CTR, ALT, SHIFT, ...
								 * coded in one 32 Bit word: 1 = pressed, 0 = not pressed
								 * LSB: 0 CTRL_LEFT
								 * 		1 CTR_RIGHT
								 * 		2 ALT_LEFT
								 * 		3 ALT_RIGHT
								 * 		4 SHIFT_LEFT
								 * 		5 SHIFT_RIGHT
								 * 		6 ...
								 * 		7 ...
								 * MSB: 31 = 1 if extended key was pressed (2 scancodes per key event)
								 */
	ConsoleBuffer *consoleBuffer;
	int hasFocus;				// = 1 if this console has the focus, else 0.
								

public:
	VirtualConsole(int consoleId, char* videoMemStart);
	~VirtualConsole();

	/**
	 * Writes the content of the buffer to the VideoBuffer.
	 */
	idlsize_t write(const buffer_t *buffer, const uint32_t pos);
	
	/*
	 * Reads the scancodes from the inputBuffer.
	 */
	CORBA_unsigned_long read(idlsize_t readsize, CORBA_char* buf);
	
	/*
	 * Adds a scancode from the keyboard to the inputBuffer of this console. The
	 * scancode is not processed or otherwise translated.
	 */
	void addInput(uint8_t c);
	
	/*
	 * Clears the input buffer;
	 */
	void ClearInputBuffer();
	
	/*
	 * Sets the current focus.
	 */
	void setFocus(int f) {
		hasFocus = f;
	}
	
	/*
	 * Refreshs the screen,
	 */
	void update() {
		consoleBuffer->update();
	}
	
	void PageUp() {
		consoleBuffer->PageUp();	
	}
	
	void PageDown() {
		consoleBuffer->PageDown();
	}



	int CTRLpressed() {
		return (keyStatus & 0x00000001) || (keyStatus & 0x00000002);
	}
	
	int ALTpressed() {
		return (keyStatus & 0x00000004) || (keyStatus & 0x00000008);
	}
	
	int SHIFTpressed() {
		return (keyStatus & 0x00000010) || (keyStatus & 0x00000020);
	}
	
	void setCTRL(int i) {
		if(i) {
			keyStatus |= 0x00000003;	
		} else {
			keyStatus &= 0xfffffffc;	
		}
	}
	
	void setALT(int i) {
		if(i) {
			keyStatus |= 0x0000000c;	
		} else {
			keyStatus &= 0xfffffff3;	
		}	
	}
		
	void setSHIFT(int i) {
		if(i) {
			keyStatus |= 0x00000030;
		} else {
			keyStatus &= 0xffffffcf;	
		}	
	}

	
};
