#ifndef UTIL_H_
#define UTIL_H_

#include <l4/types.h>

/** The following is a tricky macro to ensure a struct/class has a specific
 * size. The check is done at compiletime. The trick is that C doesn't allow
 * duplicate case labels...
 */
#define ASSERT_SIZE(mystruct, size)                                         \
    namespace { inline void mystruct##_test() {                      		\
        int i=0; switch(i) { case 0: ; case (sizeof(mystruct) == (size)): ; } \
    } }

template<typename T>
inline T min(T v1, T v2)
{
	if(v1 < v2)
		return v1;
	return v2;
}

template<typename T>
inline T max(T v1, T v2)
{
	if(v1 >= v2)
		return v1;
	return v2;
}

/**
 * Rounder better be a power of 2
 */
extern inline L4_Word_t around( L4_Word_t roundee, L4_Word_t rounder )
{
	return roundee & ( ~( rounder - 1 ) );
}

/**
 * Rounder better be a log base 2 of the actual rounder
 */
extern inline L4_Word_t aroundLb( L4_Word_t roundee, L4_Word_t rounder )
{
	return around( roundee, 1UL << rounder );
}

/**
 * Rounder better be a power of 2
 */
extern inline L4_Word_t aroundUp( L4_Word_t roundee, L4_Word_t rounder )
{
	return around( roundee + ( rounder - 1 ), rounder );
}

/**
 * Rounder better be a log base 2 of the actual rounder
 */
extern inline L4_Word_t aroundUpLb(L4_Word_t roundee, L4_Word_t rounder)
{	
	return aroundUp( roundee, 1UL << rounder );
}

/**
 *  Find most significant bit (you'll have to check for yourself wether it has any bits set)
 */
extern inline L4_Word_t msBit(L4_Word_t bits)
{
#define ARCH_IA32
#ifdef ARCH_IA32
	L4_Word_t res;
	asm (                            // the keyword volatile is not needed for this simple calculation
	    "bsr %[in], %[out]       \n" // bit scan reverse
	    "jnz 1f                  \n" // If it was not zero, jump to label 1
	    "movl $0xFFFFFFFF,%[out] \n" // so it was zero. Than let's return some kind of error code
	    "1:                      \n" // This is our label for above
	: [ out ] "=r" ( res )           // output
	: [ in ] "r" ( bits )            // input
	// :                             // clobber
	);
	
	return res;
#else

    register L4_Word_t i = ( sizeof( L4_Word_t ) * 8 - 1 );
    for ( L4_Word_t j = 1UL << ( sizeof( L4_Word_t ) * 8 - 1 ); j > 0; j >>= 1, i-- )
        if ( bits & j )
            return i;
    return ~0UL;
#endif
    }

/**
 * Find least significant bit (you'll have to check for yourself wether it has any bits set)
 */
extern inline L4_Word_t lsBit(L4_Word_t bits)
{
#ifdef ARCH_IA32
	L4_Word_t res;
	asm (                               // the keyword volatile is not needed for this simple calculation
	    "bsf %[in], %[out]       \n" // bit scan forward
	    "jnz 1f                  \n" // If it was not zero, jump to label 1
	    "movl $0xFFFFFFFF,%[out] \n" // so it was zero. Than let's return some kind of error code
	    "1:                      \n" // This is our label for above
	: [ out ] "=r" ( res )                  // output
	: [ in ] "r" ( bits )                   // input
	// :                                  // clobber
	);
	
	return res;
#else
	register L4_Word_t i = 0;
	for ( L4_Word_t j = 1UL; j < ( 1UL << ( sizeof( L4_Word_t ) * 8 - 1 ) ); j <<= 1, i++ ) {
		if ( bits & j )
			return i;
	}
	
	return ~0UL;
#endif
}

extern inline L4_Word_t numBits( L4_Word_t bits )
{
	int ret = 0;
	for ( L4_Word_t i = 1UL; i < ( 1UL << ( sizeof( L4_Word_t ) * 8 - 1 ) ); i <<= 1 ) {
		if ( bits & i )
			ret++;
	}
	return ret;
}    

#endif
