#include <config.h>

#include <stdio.h>
#include <iostream>

namespace std
{
	fstreambuf stdoutbuf(stdout);
	ostream cout(&stdoutbuf);
	ostream cerr(&stdoutbuf);
}

