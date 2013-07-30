#ifndef _TIME_H
#define _TIME_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

time_t time(time_t *t) __THROW;

double difftime(time_t time1, time_t time0) __THROW __attribute__((__const__));

#define CLOCKS_PER_SEC 1000000l

clock_t clock(void);

__END_DECLS

#endif
