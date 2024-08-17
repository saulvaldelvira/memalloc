#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__

#include <stddef.h>

/*
 * Allocates _n bytes and returns a pointer to the allocated memory.
 */
void* memalloc(size_t _n);

/*
 * Allocates memory for an array of _nmemb elements, each one of
 * _elem_size bytes.
 * The memory is properly zero initialized.
 * If the multiplication of _nmemb and _elem_size overflows, return NULL
 */
void* memcalloc(size_t _nmemb, size_t _elem_size);

/*
 * Extends the given pointer to _n bytes.
 * Returns the address of the new region, which may
 * or may not start at the same addreExtends the given pointer to _n bytes.
 * Returns the address of the new region, which may
 * or may not start at the same address.
 */
void* memrealloc(void *ptr, size_t _n);

/*
 * Same as memrealloc. Reallocs the given pointer for an array of
 * _nmemb elements of _elem_size bytes.
 * If the multiplication of _nmemb and _elem_size overflows, return NULL
 */
void* memreallocarray(void *ptr, size_t _nmemb, size_t _elem_size);

/*
 * Frees a pointer previously allocated with [memalloc], making
 * it available for future allocations.
 */
void memfree(void *ptr);

#endif /* __MEMALLOC_H__ */
