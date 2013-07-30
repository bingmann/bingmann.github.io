// $Id: unistd.h 178 2006-07-26 20:05:52Z sdi2 $

#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/cdefs.h>

__BEGIN_DECLS

// these are handy. implemented in time.c
int usleep(unsigned long useconds) __THROW;
unsigned int sleep(unsigned int seconds) __THROW;

// symbol is declared in ia32-crt0.S
extern char **environ;

// emulated by the environment variable CWD
char *getcwd(char *buf, size_t size);


int chdir(const char* path);

__END_DECLS

#endif
