#ifndef ASSERT_H_
#define ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif
	
	void __assert_fail(const char* assertion, const char* file,
	                   unsigned int line, const char* function) __attribute__((noreturn));
	
#define assert(X) { if(!(X)) __assert_fail(#X, __FILE__, __LINE__, __PRETTY_FUNCTION__); }

#ifdef __cplusplus
}
#endif

#endif
