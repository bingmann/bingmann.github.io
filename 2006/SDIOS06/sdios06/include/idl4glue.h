//
// File: include/idl4Glue.h
//
// Description: Include this before and idl4 include files
//
//
// Author: Ulf Vatter <uvatter@ira.uka.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __INCLUDE__IDLE4GLUE_H__
#define __INCLUDE__IDLE4GLUE_H__

#include <stdlib.h>

#include <idl4/corba.h>

#ifdef __cplusplus
extern "C" {
#endif

	void* CORBA_alloc(size_t size);
	void CORBA_free(void* ptr);
	CORBA_char* CORBA_string_alloc(size_t size);
	void CORBA_string_free(CORBA_char* ptr);

#ifdef __cplusplus
}
#endif

#endif


