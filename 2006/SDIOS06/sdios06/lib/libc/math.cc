#include <config.h>

#include <math.h>
#include <sdi/panic.h>

extern "C"
{

	double  pow ( double mant, double expo )
	{
            // XXX Ultra nasty hack here: gcc has a builtin pow function,
            // so this one should never be called...
            panic("sdi pow called");

            return 0;
	}

}
