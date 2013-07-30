// $Id: pdebug.h 66 2006-07-17 21:30:52Z sdi2 $
// some simple macros for the myriad debug messages in the pager

#ifndef SDI2_PAGER_DEBUG_H
#define SDI2_PAGER_DEBUG_H

static const bool show_debug0 = true;
static const bool show_debug1 = false;
static const bool show_debug2 = false;
static const bool show_debug3 = false;

#define pdbgX(cond, ...)	do { if (cond) { printf(__VA_ARGS__); } } while(0)

#define pdbg0(...)			pdbgX(show_debug0, __VA_ARGS__)
#define pdbg1(...)			pdbgX(show_debug1, __VA_ARGS__)
#define pdbg2(...)			pdbgX(show_debug2, __VA_ARGS__)
#define pdbg3(...)			pdbgX(show_debug3, __VA_ARGS__)

#endif // SDI2_PAGER_DEBUG_H
