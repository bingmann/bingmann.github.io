// $Id$
// trivial and utterly wrong time() implementation, but useful sleep() and
// usleep()

#include <time.h>

#include <l4/types.h>
#include <l4/schedule.h>
#include <l4/ipc.h>

time_t time(time_t *t)
{
	L4_Clock_t clk = L4_SystemClock();

	// system clock is in milliseconds
	clk.raw /= 10000000;

	if (t) *t = (time_t)clk.raw;

	return (time_t)clk.raw;
}

double difftime(time_t time1, time_t time2) {
	return (double)time1 - (double)time2;
}

clock_t clock(void) {
	return L4_SystemClock().raw;
}

int usleep(unsigned long useconds)
{
	L4_Sleep( L4_TimePeriod((L4_Word64_t)useconds) );
	return useconds;
}

unsigned int sleep(unsigned int seconds) {
	usleep(seconds * 1000000);
	return seconds;
}
