#include <stdatomic.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include "os.h"
#include "debug.h"
#include "memalloc.h"
#include <limits.h>

typedef unsigned char u8;
#define offset(ptr, bytes) ((void*)((u8*)(ptr) + bytes))

#define would_overflow(a,b) (a > SIZE_MAX / b)

/* =================== CHUNK ========================== */

typedef struct chunk {
        size_t len;
        u8 available;
        struct chunk *next, *prev;
} chunk_t;

static chunk_t *head = NULL;

#define HEADER_SIZE sizeof(chunk_t)
#define NEW_CHUNK_MULTIPLIER 2

#define GET_CHUNK_DATA(c) offset(c, HEADER_SIZE)
#define get_chunk(ptr) offset(ptr, (-HEADER_SIZE))

static chunk_t* new_chunk(size_t n) {
        chunk_t *c = request_os_mem(n);
        if (!c) return NULL;
        *c = (chunk_t){
                .available = 1,
                .len = n,
        };
        DEBUG("Creating new chunk of [%lu] bytes\n", n);
        return c;
}

#define __merge_chunks(c,r) { \
        c->len += r->len + HEADER_SIZE; \
        c->next = r->next; \
        if (c->next) \
        c->next->prev = c; } \

static chunk_t* merge_adjacent(chunk_t *l) {
        chunk_t *curr = l;

        while (curr && curr->next) {
                if (!curr->next->available)
                        break;
                __merge_chunks(curr, curr->next);
                curr = curr->next;
        }
        curr = l;
        chunk_t *prev = curr;
        while (curr && curr->prev) {
                if (!curr->prev->available)
                        break;
                __merge_chunks(curr->prev, curr);
                prev = curr;
                curr = curr->prev;
        }
        return prev;
}

#undef __merge_chunks

/* ==================================================== */

/* ======== SYNCRONIZATION =========== */

static mtx_t MEMALLOC_LOCK;

#define TRANSACTION(body) { \
        mtx_lock(&MEMALLOC_LOCK); \
        body; \
        mtx_unlock(&MEMALLOC_LOCK); }

/* =================================== */

/* =============  Memory alignment  =======================*/

#define ALIGN_SIZE sizeof(max_align_t)
#define ALIGN(n) (((n) + (ALIGN_SIZE-1)) & ~(ALIGN_SIZE-1))

/* ======================================================== */

static int __init(void) {
        mtx_init(&MEMALLOC_LOCK, mtx_plain);
        head = new_chunk((1024 * 1024) + HEADER_SIZE);
        return head != NULL;
}

/* =================== MEMALLOC ================================================= */

static chunk_t* find_fit(size_t _n) {
        chunk_t *chunk = head, *prev = NULL;
        while (chunk) {
                if (chunk->available && chunk->len >= _n)
                        break;
                prev = chunk;
                chunk = chunk->next;
        }
        if (!chunk) {
                chunk = new_chunk((_n + HEADER_SIZE) * NEW_CHUNK_MULTIPLIER);
                if (!chunk)
                        return NULL;
                if (prev)
                        prev->next = chunk;
                chunk->prev = prev;
        }
        return chunk;
}

static void* get_ptr_from_chunk(chunk_t *t, size_t _n) {
        if (!t) return NULL;
        if (t->len > (_n + _n / 4)) {
                size_t rem_len = t->len - _n - HEADER_SIZE;
                chunk_t *remaining = offset(t, HEADER_SIZE + _n);
                *remaining = (chunk_t) {
                        .len = rem_len,
                        .prev = t,
                        .available = 1,
                };
                t->next = remaining;
                t->len = _n;
        }
        t->available = 0;
        return GET_CHUNK_DATA(t);
}

void* memalloc(size_t _n) {
        static atomic_bool __is_init = false;
        if (!__is_init) {
                if (!__init())
                        return NULL;
                __is_init = true;
        }
        _n = ALIGN(_n);
        void *ptr;

TRANSACTION (
        chunk_t *c = find_fit(_n);
        if (c == NULL)
                ptr = NULL;
        else
                ptr = get_ptr_from_chunk(c, _n);
)

        return ptr;
}

void* memcalloc(size_t _nmemb, size_t _elem_size) {
        if (would_overflow(_nmemb, _elem_size))
                return NULL;
        void *ptr;
        TRANSACTION( ptr = memalloc(_nmemb * _elem_size) );
        if (ptr)
                memset(ptr, 0, _nmemb * _elem_size);
        return ptr;
}

void* memrealloc(void *ptr, size_t _n) {
        if (!ptr)
                return memalloc(_n);
        if (_n == 0) {
                memfree(ptr);
                return NULL;
        }
        chunk_t *c = get_chunk(ptr);
        size_t prev_len = c->len;

TRANSACTION (
        c->available = 1;
        c = merge_adjacent(c);

        chunk_t *newc;
        if (c->len < _n)
                newc = c;
        else
                newc = find_fit(_n);

        void *dst = get_ptr_from_chunk(newc, _n);
        if (dst != ptr)
                memmove(dst, ptr, prev_len);
        ptr = dst;
)
        return ptr;
}

void* memreallocarray(void *ptr, size_t _nmemb, size_t _elem_size) {
        if (would_overflow(_nmemb, _elem_size))
                return NULL;
        return memrealloc(ptr, _nmemb * _elem_size);
}

/* ============================================================================= */

/* =================== MEMFREE ================================================= */

void memfree(void *ptr) {
TRANSACTION(
        chunk_t *c = get_chunk(ptr);
        c->available = 1;
        merge_adjacent(c);
)
}
/* ============================================================================= */
