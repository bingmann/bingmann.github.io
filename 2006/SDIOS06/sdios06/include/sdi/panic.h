/*
 * $Id: panic.h 69 2006-07-17 23:23:17Z sdi2 $
 */
#ifndef PANIC_H_
#define PANIC_H_

#ifdef __cplusplus
extern "C" {
#endif

	void __panic(const char* message, const char* function,
	             const char* file, unsigned int line) __attribute__((noreturn));

#define panic(x)	{ __panic(x, __PRETTY_FUNCTION__, __FILE__, __LINE__); }

#ifdef __cplusplus	
}
#endif

#endif
