// $Id: GraphPort.h 101 2006-04-23 08:19:17Z bingmann $

#ifndef VGS_GraphPort_H
#define VGS_GraphPort_H

/** \file
 * Porting header file to make the library compile both, gcc and visual c++
 */

namespace VGServer {

#if defined(__GNUC__)

inline int g_strcasecmp(const char *a, const char *b)
{
    return strcasecmp(a,b);
}

#define g_snprintf	snprintf

#else

inline g_strcasecmp(const char *a, const char *b)
{
    return _stricmp(a,b);
}

#define g_snprintf	_snprintf_s

#endif


} // namespace VGServer

#endif // VGS_GraphPort_H
