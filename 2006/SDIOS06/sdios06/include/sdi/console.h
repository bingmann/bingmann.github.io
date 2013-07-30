#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <l4/types.h>

#include <idl4glue.h>
#include <if/types.h>

#ifdef __cplusplus
extern "C" {
#endif

	// Translates scancode to printable ascii, 'b', '\n'
	// else returns 0
	char ScancodeToAscii(uint8_t scancode, uint32_t keyStatus);

	// Returns 1 if the scancodes correspondes to a printable ascii.
	// 32 <= printable ascii <= 126
	L4_Bool_t isPrintable(uint8_t scancode, uint32_t keyStatus);

	//Writes to the console like printf.
	void writeConsole(const char* message, ...)
		__attribute__((format(printf, 1, 2)));

	// Reads # readsize scancodes from the input buffer of the console
	// and saves it into the buffer.
	void readConsole(const idlsize_t readsize, buffer_t *buffer);

#ifdef __cplusplus
}
#endif


#endif /*CONSOLE_H_*/
