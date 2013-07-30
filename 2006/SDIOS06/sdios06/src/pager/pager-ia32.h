// $Id: pager-ia32.h 60 2006-07-17 09:55:43Z sdi2 $

// *** x86 assembler memset functions for clearing fpages

static inline void* pager_memset(void *s, char c, unsigned int count)
{
	int d0, d1;
	asm volatile ( "cld; rep; stosb		\n\t"
				   : "=&c" (d0), "=&D" (d1)
				   : "a" (c), "1" (s), "0" (count)
				   : "memory");
	return s;
}

static inline void* pager_memset_long(void *s, unsigned int c, unsigned int count)
{
	int d0, d1;
	asm volatile ( "cld; rep; stosl		\n\t"
				   : "=&c" (d0), "=&D" (d1)
				   : "a" (c), "1" (s), "0" (count / 4)
				   : "memory");
	return s;
}
