#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__

#include <stddef.h>

#ifdef __has_attribute
#if __has_attribute(malloc)
#       define __attr_malloc__ __attribute__((malloc))
#else
#       define __attr_malloc__
#endif
#endif

/*
 * Allocates _n bytes and returns a pointer to the allocated memory.
 */
__attr_malloc__
void* memalloc(size_t _n);

/*
 * Allocates memory for an array of _nmemb elements, each one of
 * _elem_size bytes.
 * The memory is properly zero initialized.
 * If the multiplication of _nmemb and _elem_size overflows, return NULL
 */
__attr_malloc__
void* memcalloc(size_t _nmemb, size_t _elem_size);

/*
 * Extends the given pointer to _n bytes.
 * Returns the address of the new region, which may
 * or may not start at the same addreExtends the given pointer to _n bytes.
 * Returns the address of the new region, which may
 * or may not start at the same address.
 */
__attr_malloc__
void* memrealloc(void *ptr, size_t _n);

/*
 * Same as memrealloc. Reallocs the given pointer for an array of
 * _nmemb elements of _elem_size bytes.
 * If the multiplication of _nmemb and _elem_size overflows, return NULL
 */
__attr_malloc__
void* memreallocarray(void *ptr, size_t _nmemb, size_t _elem_size);

/*
 * Frees a pointer previously allocated with [memalloc], making
 * it available for future allocations.
 */
void memfree(void *ptr);

size_t memalloc_get_n_mallocs(void);
size_t memalloc_get_n_frees(void);
size_t memalloc_get_currently_allocated_ptrs(void);

void memalloc_reset(void);

#endif /* __MEMALLOC_H__ */
