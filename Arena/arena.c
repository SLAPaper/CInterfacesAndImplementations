#include <stdlib.h>
#include <string.h>
#include "../Assert/Assert.h"
#include "../Except/Except.h"
#include "Arena.h"
#define T Arena_T

const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed = { "Arena Allocation Failed" };

//macros 71
#define THRESHOLD 10

//types 67
struct T {
    T prev;
    char *avail;
    char *limit;
};

union align {
    int i;
    long l;
    long long ll;
    long *lp;
    long long *llp;
    void *p;
    void(*fp)(void);
    float f;
    double d;
    long double ld;
};

union header {
    struct T b;
    union align a;
};

//data 70
static T freechunks;
static int nfree;

//functions 68
T Arena_new(void) {
    T arena = malloc(sizeof(*arena));
    if (arena == NULL)
        RAISE(Arena_NewFailed);
    arena->prev = NULL;
    arena->limit = arena->avail = NULL;
    return arena;
}

void Arena_dispose(T *ap) {
    assert(ap && *ap);
    Arena_free(*ap);
    free(*ap);
    *ap = NULL;
}

void * Arena_alloc(T arena, long nbytes, const char *file, int line) {
    assert(arena);
    assert(nbytes > 0);

    nbytes = ((nbytes + sizeof(union align) - 1) / sizeof(union align)) * sizeof(union align);
    
    // Exercise 6.1
    // Go throgh other arenas
    T arena_temp = arena;
    while (arena_temp && nbytes > arena_temp->limit - arena_temp->avail)
        arena_temp = arena_temp->prev;
    if (arena_temp)
        arena = arena_temp;

    while (nbytes > arena->limit - arena->avail) {
        T ptr;
        char *limit;
        if ((ptr = freechunks) != NULL) {
            freechunks = freechunks->prev;
            --nfree;
            limit = ptr->limit;
        }
        else {
            long long m = sizeof(union header) + nbytes + 10 * 1024;
            ptr = malloc(m);
            if (ptr == NULL) {
                if (file == NULL)
                    RAISE(Arena_Failed);
                else
                    Except_raise(&Arena_Failed, file, line);
            }
            limit = (char *)ptr + m;
        }
        *ptr = *arena;
        arena->avail = (char *)((union header *)ptr + 1);
        arena->limit = limit;
        arena->prev = ptr;
    }
    arena->avail += nbytes;
    return arena->avail - nbytes;
}

void * Arena_calloc(T arena, long count, long nbytes, const char *file, int line) {
    assert(count > 0);

    void *ptr = Arena_alloc(arena, count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);
    return ptr;
}

void Arena_free(T arena) {
    assert(arena);
    while (arena->prev) {
        struct T tmp = *arena->prev;
        if (nfree < THRESHOLD) {
            arena->prev->prev = freechunks;
            freechunks = arena->prev;
            ++nfree;
            freechunks->limit = arena->limit;
        }
        else
            free(arena->prev);
        *arena = tmp;
    }
    assert(arena->limit == NULL);
    assert(arena->avail == NULL);
}

#undef T