#ifndef ALLOC_H
#define ALLOC_H

#include "tinyalloc.h"

inline void *
malloc(size_t size)
{
    return ta_alloc(size);
}

inline void *
calloc(size_t num, size_t size)
{
    return ta_calloc(num, size);
}

inline void
free(void *ptr)
{
    ta_free(ptr);
}

#endif /* ALLOC_H */
