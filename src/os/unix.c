#include "../os.h"
#include <unistd.h>
#include <wchar.h>

#ifdef MEMALLOC_MAX_OS_MEM
#include "../debug.h"
static size_t allocated_size;
#endif

static void* initial_brk = NULL;

void* request_os_mem(size_t len) {
#ifdef MEMALLOC_MAX_OS_MEM
        if (allocated_size + len >= MEMALLOC_MAX_OS_MEM) {
                DEBUG("[%ld] exceeds max allocated size [%ld]\n", allocated_size + len, MEMALLOC_MAX_OS_MEM);
                return NULL;
        }
#endif
        void *ptr = sbrk(len);
        if (!initial_brk)
                initial_brk = ptr;
        if (ptr == (void*)-1)
                ptr = NULL;

#ifdef MEMALLOC_MAX_OS_MEM
        allocated_size += len;
#endif
        return ptr;
}


void reset_os_mem(void) {
        brk(initial_brk);
}
