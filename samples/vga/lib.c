#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// NOTE: Inefficient implementation
void *
memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *destb = (uint8_t *) dest;
    uint8_t *srcb = (uint8_t *) src;
    while (n--) {
      *destb = *srcb;
      destb++;
      srcb++;
    }
    return dest;
}

int
isalpha(int c)
{
	return ((unsigned)c | 32) - 'a' < 26;
}

int
isdigit(int c)
{
	return (unsigned)c - '0' < 10;
}

int
isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

// NOTE: Inefficient implementation
size_t
strlen(const char* str)
{
    size_t len = 0;
    while (1) {
        if (*str == 0) {
            return len;
        }
        str++;
    }
}

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)

char *
__strchrnul(const char* s, int c)
{
	const size_t* w;
	size_t k;
	c = (unsigned char)c;

	if(!c)
	{
		return (char*)(uintptr_t)s + strlen(s);
	}

	for(; (uintptr_t)s % ALIGN; s++)
	{
		if(!*s || *(const unsigned char*)s == c)
		{
			return (char*)(uintptr_t)s;
		}
	}

	k = ONES * (unsigned long)c;

	for(w = (const void*)s; !HASZERO(*w) && !HASZERO(*w ^ k); w++)
	{
		;
	}
	for(s = (const void*)w; *s && *(const unsigned char*)s != c; s++)
	{
		;
	}

	return (char*)(uintptr_t)s;
}

char *
strchr(const char* s, int c)
{
	char* r = __strchrnul(s, c);
	return *(unsigned char*)r == (unsigned char)c ? r : 0;
}

char *
strcpy(char* __restrict dst, const char* __restrict src)
{
	const size_t length = strlen(src);
	//  The stpcpy() and strcpy() functions copy the string src to dst
	//  (including the terminating '\0' character).
	memcpy(dst, src, length + 1);
	//  The strcpy() and strncpy() functions return dst.
	return dst;
}

int
strcmp(const char* s1, const char* s2)
{
	int r = -1;

	if(s1 == s2)
	{
		// short circuit - same string
		return 0;
	}

	// I don't want to panic with a NULL ptr - we'll fall through and fail w/ -1
	if(s1 != NULL && s2 != NULL)
	{
		// iterate through strings until they don't match or s1 ends (null-term)
		for(; *s1 == *s2; ++s1, ++s2)
		{
			if(*s1 == 0)
			{
				r = 0;
				break;
			}
		}

		// handle case where we didn't break early - set return code.
		if(r != 0)
		{
			r = *(const char*)s1 - *(const char*)s2;
		}
	}

	return r;
}
