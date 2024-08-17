#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef MEMALLOC_DEBUG
#include <stdio.h>
#define DEBUG(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG(fmt, ...)
#endif

#endif /* __DEBUG_H__ */
