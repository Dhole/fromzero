#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>

__attribute__((weak, noreturn)) void abort(void)
{
	while(1) {};
}

__attribute__((weak, noreturn)) void exit(int code)
{
	while(1) {};
}

__attribute__((noreturn, weak)) void __assert_fail(const char* expr, const char* file,
                                                   unsigned int line, const char* function)
{
	// printf("Assertion failed: %s (%s: %s: %u)\n", expr, file, function, line);
	abort();
}

#ifdef NDEBUG
#define assert(x) (void)0
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__), 0)))
#endif

void * memcpy(void *dest, const void *src, size_t n);
int isalpha(int c);
int isalnum(int c);
int isdigit(int c);
size_t strlen(const char* str);
char * strchr(const char* s, int c);
char * strcpy(char* __restrict dst, const char* __restrict src);
int strcmp(const char* s1, const char* s2);

#endif /* LIB_H */
