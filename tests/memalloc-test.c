#include "../include/memalloc.h"
#include "test.h"
#include <string.h>

int main(void) {
        test_start("Memalloc Test 1");

        const int N = 1024;
        const int ITER = 10;
        int* ptrs[ITER];
        for (int i = 0; i < ITER; i++) {
                int *ptr = memalloc(sizeof(int[N]));
                for (int i = 0; i < N; i++) {
                        ptr[i] = i;
                }
                ptrs[i] = ptr;
                /* memfree(ptr); THIS IS BAD */
        }

        assert(memalloc_get_currently_allocated_ptrs() == ITER);
        assert(memalloc_get_n_mallocs() == ITER);
        assert(memalloc_get_n_frees() == 0);

        /* Ask for memory in between, to make sure that the
         * arrays above are respected, and no memory is corrupted.*/
        void *v = memalloc(N * ITER * 2);
        memset(v, 0, N * ITER * 2);
        memfree(v);

        assert(memalloc_get_n_mallocs() == ITER + 1);

        for (int i = 0; i < ITER; i++) {
                int *ptr = ptrs[i];
                for (int i = 0; i < N; i++) {
                        assert(ptr[i] == i);
                }

                memfree(ptr);
        }

        assert(memalloc_get_n_frees() == ITER + 1);
        assert(memalloc_get_currently_allocated_ptrs() == 0);
}
