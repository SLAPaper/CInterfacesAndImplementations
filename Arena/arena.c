#include <stdlib.h>
#include <string.h>
#include "../Assert/Assert.h"
#include "../Except/Except.h"
#include "Arena.h"

const Except_T Arena_NewFailed = { "Arena Creation Failed" };
const Except_T Arena_Failed = { "Arena Allocation Failed" };

//macros 71
#define THRESHOLD 10

//types 67
struct Arena_T {
    Arena_T prev;
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
    struct Arena_T b;
    union align a;
};

//data 70
static Arena_T freechunks;
static int nfree;

// Exercise 6.2
// Descend order
static int Arena_compare(const void *arena1, const void *arena2) {
    const struct Arena_T *arena1_t = arena1;
    const struct Arena_T *arena2_t = arena2;
    long long result = arena1_t->avail - arena1_t->limit - (arena2_t->avail - arena2_t->limit);
    if (result > 0)
        return -1;
    else if (result < 0)
        return 1;
    else
        return 0;
}

static int Arena_count(Arena_T arena) {
    int count = 0;
    Arena_T ptr = arena->prev;
    while (ptr) {
        ++count;
        ptr = ptr->prev;
    }
    return count;
}

static void Arena_sort(Arena_T arena) {
    int n = Arena_count(arena);
    Arena_T *arraylist = calloc(n, sizeof(Arena_T));
    if (arraylist == NULL) {
        RAISE(Arena_Failed);
    }

    Arena_T ptr = arena;
    for (int i = 0; i < n; ++i, ptr = ptr->prev) {
        arraylist[i] = ptr;
    }
    qsort(arraylist, n, sizeof(Arena_T), Arena_compare);
    arena->prev = arraylist[0];
    for (int i = 1; i < n; ++i) {
        arraylist[i - 1]->prev = arraylist[i];
    }
    arraylist[n - 1]->prev = NULL;
    free(arraylist);
}

//functions 68
Arena_T Arena_new(void) {
    Arena_T arena = malloc(sizeof(*arena));
    if (arena == NULL)
        RAISE(Arena_NewFailed);
    arena->prev = NULL;
    arena->limit = arena->avail = NULL;
    return arena;
}

void Arena_dispose(Arena_T *ap) {
    assert(ap && *ap);
    Arena_free(*ap);
    free(*ap);
    *ap = NULL;
}

void * Arena_alloc(Arena_T arena, long nbytes, const char *file, int line) {
    assert(arena);
    assert(nbytes > 0);

    nbytes = ((nbytes + sizeof(union align) - 1) / sizeof(union align)) * sizeof(union align);

    // Exercise 6.1
    // Go throgh other arenas
    /*
    Arena_T arena_temp = arena;
    while (arena_temp && nbytes > arena_temp->limit - arena_temp->avail)
        arena_temp = arena_temp->prev;
    if (arena_temp)
        arena = arena_temp;
    */

    //Exercise 6.2
    // Sort arena and find the bigest one
    Arena_sort(arena);

    if (nbytes > arena->limit - arena->avail) {
        Arena_T ptr;
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

void * Arena_calloc(Arena_T arena, long count, long nbytes, const char *file, int line) {
    assert(count > 0);

    void *ptr = Arena_alloc(arena, count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);
    return ptr;
}

void Arena_free(Arena_T arena) {
    assert(arena);
    while (arena->prev) {
        struct Arena_T tmp = *arena->prev;
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