//
// File:  src/test/main.cc
//
// Description: Testclient
//
#include <config.h>

#include <stdio.h>
#include <l4/thread.h>
#include <l4/kip.h>
#include <l4/schedule.h>

#include <sdi/log.h>
#include <sdi/locator.h>
#include <sdi/panic.h>
#include <sdi/console.h>	// ist noch nicht fertig!

#include <new>
#include <idl4glue.h>
#include <if/iflocator.h>
#include <if/iflogging.h>
#include <if/ifmemory.h>
#include <if/iffile.h>


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
							 
L4_Bool_t reading = 1;		// needed for the reading loop 
L4_ThreadId_t consoleid = L4_nilthread;
CORBA_Environment env (idl4_default_environment);

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

void ProcessInput(uint8_t scancode) {
	if(0x01 == scancode) {			// ESC - special scancodes to control the screen follow
		LogMessage("ESC - process screen control sequence");
		reading = 0;
		return;
		
	} else if(0xe0 == scancode) {	// extended scancode, the next scancode is the real key
		keyStatus |= 0x10000000;
		return;
	
	// key released (scancode hast the MSB set, = scancode (key pressed) + 128
	// reset corresponding data -> keyStatus
	} else if(scancode & 0x80) {
		//LogMessage("Key released");
		if((0xaa == scancode) || (0xb6 == scancode)) {
			// reset Shift bit 
			setSHIFT(0);
			//LogMessage("SHIFT released");
		} else if(0xb8 == scancode) {
			// reset ALT bit
			setALT(0);	
			//LogMessage("ALT released");
		} else if(0x9d == scancode) {
			// reset CTRL bit
			setCTRL(0);
			//LogMessage("CTRL released");
		}		
	
	// key pressed, set corresponding bits		
	} else {
		//LogMessage("Key pressed");
		if((0x2a == scancode) || (0x36 == scancode)) {
			setSHIFT(1);
			
		} else if(0x38 == scancode) {
			setALT(1);	

		} else if(0x1d == scancode) {
			setCTRL(1);

		} else if(0x0e == scancode) {
			// handle BACKSPACE
			writeConsole("\b");	
			
		} else if(0x1c == scancode) {
			// handle RETURN
			writeConsole("\n");	
			
		} else if(0x4b == scancode) {
			// handle LEFT
			writeConsole("%c[1D", 27);
			
		} else if(0x4d == scancode) {
			// handle RIGHT
			writeConsole("%c[1C", 27);
			
		} else if(0x53 == scancode) {
			// handle DEL
			writeConsole("%c", 127);

		} else if(0x02 ==scancode  && CTRLpressed()) {
			writeConsole("%c[12C", 27);
			
		} else if(0x03 ==scancode  && CTRLpressed()) {
			writeConsole("%c[12D", 27);
						
		} else if(0x04 ==scancode  && CTRLpressed()) {
			writeConsole("%c[123C", 27);

		} else if(0x05 ==scancode  && CTRLpressed()) {
			writeConsole("%c[123D", 27);

		} else if(0x06 ==scancode  && CTRLpressed()) {
			writeConsole("%c[31m", 27);

		} else if(0x07 ==scancode  && CTRLpressed()) {
			writeConsole("%c[0K", 27);

		} else if(0x08 ==scancode && CTRLpressed()) {
			writeConsole("%c[1K", 27);

		} else if(0x09 ==scancode && CTRLpressed()) {
			writeConsole("%c[2K", 27);

		} else if(0x0a ==scancode && CTRLpressed()) {
			writeConsole("%c[s", 27);

		} else if(0x0b ==scancode && CTRLpressed()) {
			writeConsole("%c[u", 27);

		} else {
		// if reached here, the scancode is probably no special key, but a printable character
		// so translate it to ascii
			if(isPrintable(scancode, keyStatus)) {
				// convert scancode to ascii with console lib
				char c = ScancodeToAscii(scancode, keyStatus);
				// write it to console
				writeConsole("%c", c);
			}
		}

	
	}
} // end of ProcessInput


int main()
{
	printf ("Testclient1 is alive as thread %lx\n", L4_Myself().raw);
	
	// set environment
	setenv("console#", "/console0", 0);

	/*
	 * Test console server
	 */
	FILE* console = NULL;
	while(console == NULL) {
		console = fopen("/console3", "2");
		L4_Yield();
	}
	fprintf(console, "Woha I was a printf %d\n", 42);
	fprintf(console, "ME TOO\n");	
	fclose(console);
	writeConsole("Press ESC to quit!\n");
	
	// prepare a buffer for the content to read from the console
	size_t readsize = 256;
	buffer_t inbuf;
	inbuf._buffer = new CORBA_char[readsize];
	inbuf._length = readsize;
	inbuf._maximum = readsize;	// wird von IDL4 ignoriert?
	
	while(reading) {
		// read scancode from console-server
		readConsole(readsize, &inbuf);
		
		if(inbuf._length > 0) {
			for(uint32_t i=0; i<inbuf._length; i++) {
				uint8_t scancode = inbuf._buffer[i];
				ProcessInput(scancode);
			}
		}
	}

     
	printf ("Testclient1 %lx finished.\n", L4_Myself().raw);

	return 0;
}
