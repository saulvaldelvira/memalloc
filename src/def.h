#ifndef __DEF_H__
#define __DEF_H__

typedef unsigned char u8;

#define offset(ptr, bytes) ((void*)((u8*)(ptr) + bytes))

/* =============  Memory alignment  =======================*/

#define ALIGN_SIZE sizeof(max_align_t)
#define ALIGN(n) (((n) + (ALIGN_SIZE-1)) & ~(ALIGN_SIZE-1))

/* ======================================================== */

#ifdef MEMALLOC_DEBUG
#include <stdio.h>
#define DEBUG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#endif

#define offset(ptr, bytes) ((void*)((u8*)(ptr) + bytes))

#endif /* __DEF_H__ */
