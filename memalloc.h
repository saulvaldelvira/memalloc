#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__

#include <stddef.h>

/*
 * Request _n bytes, aligned to the maximun possible size.
 */
void* memalloc(size_t _n);

/*
 * Frees a pointer previously allocated with [memalloc], making
 * it available for future allocations.
 */
void memfree(void *ptr);

#endif /* __MEMALLOC_H__ */
