#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memalloc.h"

void do_stuff(
        void* (*malloc_like)(size_t),
        void (*free_like)(void*)
) {
        const int N = 0xFFFF;
        int **buf = malloc_like(N * sizeof(int*));
        for (size_t i = 0; i < N; i++) {
                int *n = malloc_like(sizeof(int));
                *n = i;
                buf[i] = n;
        }

        // for (size_t i = 0; i < N; i++) {
        //         free_like(buf[i]);
        // }
        // free_like(buf);
}

int main() {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        do_stuff(memalloc, memfree);
        clock_gettime(CLOCK_MONOTONIC, &end);

        long elapsed = (end.tv_sec - start.tv_sec) * 1000
                + (end.tv_nsec - start.tv_sec) / 1000000;

        printf("memalloc: %ldms\n", elapsed);

        clock_gettime(CLOCK_MONOTONIC, &start);
        do_stuff(malloc, free);
        clock_gettime(CLOCK_MONOTONIC, &end);

        elapsed = (end.tv_sec - start.tv_sec) * 1000
                + (end.tv_nsec - start.tv_sec) / 1000000;

        printf("stdlib malloc: %ldms\n", elapsed);


}
