#ifndef __OS_H__
#define __OS_H__

#include <stddef.h>

/**
 * Request the os to extend de data segment by _n bytes.
 * Returns: The previous data segment offset.
 */
void* request_os_mem(size_t _n);

void reset_os_mem(void);

#endif /* __OS_H__ */
