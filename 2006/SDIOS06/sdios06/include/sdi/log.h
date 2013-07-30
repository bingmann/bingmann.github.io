/*
 * $Id: log.h 69 2006-07-17 23:23:17Z sdi2 $
 */
#ifndef LOG_H_
#define LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

	void LogMessage (const char* message, ...)
		__attribute__((format(printf, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif
