#include <new>

#include <stdint.h>

#include <sdi/log.h>
#include <sdi/console.h>
#include <sdi/panic.h>

#include "shell.h"

void showlogo()
{
#define S(x)	writeConsole("%s", x)
	S("\33[32m      ___           ___                                ___           ___\n");
	S("     /\\  \\         /\\  \\          ___                 /\\  \\         /\\  \\\n");
	S("    /::\\  \\       /::\\  \\        /\\  \\               /::\\  \\       /::\\  \\\n");
	S("   /:/\\ \\  \\     /:/\\:\\  \\       \\:\\  \\             /:/\\:\\  \\     /:/\\ \\  \\\n");
	S("  _\\:\\~\\ \\  \\   /:/  \\:\\__\\      /::\\__\\           /:/  \\:\\  \\   _\\:\\~\\ \\  \\\n");
	S(" /\\ \\:\\ \\ \\__\\ /:/__/ \\:|__|  __/:/\\/__/          /:/__/ \\:\\__\\ /\\ \\:\\ \\ \\__\\\n");
	S(" \\:\\ \\:\\ \\/__/ \\:\\  \\ /:/  / /\\/:/  /             \\:\\  \\ /:/  / \\:\\ \\:\\ \\/__/\n");
	S("  \\:\\ \\:\\__\\    \\:\\  /:/  /  \\::/__/               \\:\\  /:/  /   \\:\\ \\:\\__\\\n");
	S("   \\:\\/:/  /     \\:\\/:/  /    \\:\\__\\                \\:\\/:/  /     \\:\\/:/  /\n");
	S("    \\::/  /       \\::/__/      \\/__/                 \\::/  /       \\::/  /\n");
	S("     \\/__/         ~~                                 \\/__/         \\/__/\n");
	S("\n");
	S("   \33[31m(c) 2006    Timo Bingmann, Matthias Braun, Torsten Geiger, Andreas Maehler\n");
	S("\33[00m\n");
	S("\n");
}

int main()
{
	LogMessage( "[shell] Shell is alive..." );

	if (getenv("STDIN") == NULL)
	{
		putenv("STDIN=/console0");
		putenv("STDOUT=/console0");
		putenv("STDERR=/console0");
	}

	showlogo();

	writeConsole("yash: SDIOS Shell 0.1 : Welcome\n" );
	
	( new Shell() )->Run();
	
	writeConsole("yash: exiting shell!\n");

	return 0;
}
