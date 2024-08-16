#include <stdatomic.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <threads.h>
#include <time.h>
#include "os.h"
#include "debug.h"

/* ============ CHUNK =============== */
typedef unsigned char u8;

typedef struct chunk {
        size_t len;
        u8 available;
        struct chunk *next, *prev;
} chunk_t;

static chunk_t *head = NULL;

#define HEADER_SIZE sizeof(chunk_t)
#define NEW_CHUNK_MULTIPLIER 2

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

/* =================================== */

/* ======== SYNCRONIZATION =========== */
static mtx_t MEMALLOC_LOCK;

#define TRANSACTION(body) { \
        mtx_lock(&MEMALLOC_LOCK); \
        body \
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
                chunk_t *remaining = (chunk_t*)((uintptr_t)t + HEADER_SIZE + _n);
                *remaining = (chunk_t) {
                        .len = rem_len,
                        .prev = t,
                        .available = 1,
                };
                t->next = remaining;
        }
        t->available = 0;
        return (void*)((uintptr_t)t + HEADER_SIZE);
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

/* ============================================================================= */

/* =================== MEMFREE ================================================= */

chunk_t* try_merge(chunk_t *l, chunk_t *r) {
        if (!l || !l->available)
                return r;
        if (!r || !r->available)
                return l;
        l->len += r->len + HEADER_SIZE;
        l->next = r->next;
        return l;
}

void memfree(void *ptr) {
TRANSACTION(
        chunk_t *c = (chunk_t*)((uintptr_t)ptr - HEADER_SIZE);
        c->available = 1;
        c = try_merge(c->prev, c);
        try_merge(c, c->next);
)
}

/* ============================================================================= */
