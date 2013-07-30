#ifndef SHELL_H_
#define SHELL_H_

#include <l4/types.h>

#include <idl4glue.h>
#include <if/types.h>


const uint32_t FLAG_EXTENDED = 0x10000000;
const uint32_t FLAG_CTRL     = 0x00000003;
const uint32_t FLAG_ALT      = 0x0000000c;
const uint32_t FLAG_SHIFT    = 0x00000030;


const char     PROMPT_CHAR    = '>';
const uint16_t CWDPATH_LENGTH = 256;
const size_t   COMMAND_LENGTH = 256;
const uint8_t  NUM_COMMANDS   = 16;


class Shell
{
private:
	buffer_t inbuf;
	uint32_t keyStatus;
	uint16_t cursorpos;

	char    command[NUM_COMMANDS][COMMAND_LENGTH];
	uint8_t current_command;
	uint8_t display_command;
	
	bool	terminate;
		
	void WritePrompt();
	void DisplayToCurrent();
	void ProcessInput( uint8_t scancode );
	void Execute();

	
public:
	 Shell();
	~Shell();

	void Run( void );
};



#endif /*SHELL_H_*/
