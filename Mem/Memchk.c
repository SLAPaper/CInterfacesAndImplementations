#include <stdlib.h>
#include <string.h>
#include "../Assert/Assert.h"
#include "../Except/Except.h"
#include "Mem.h"

//checking types 58
union align {
    int i;
    long l;
    long *lp;
    long long ll;
    void *p;
    void(*fp)(void);
    float f;
    double d;
    long double ld;
};

//checking macros 58
#define hash(p, t) (((unsigned long)(p) >> 3) & (sizeof(t) / sizeof((t)[0]) - 1))
#define NDESCRIPTORS 512
#define NALLOC ((4096 + sizeof(union align) -1 ) / (sizeof(union align))) * (sizeof(union align))

//data 54
const Except_T Mem_Failed = { "Allocation Failed" };

//checking data 56
static struct descriptor
{
    struct descriptor *free;
    struct descriptor *link;
    const void *ptr;
    long size;
    const char *file;
    int line;
} *htab[2048];

static struct descriptor freelist = { &freelist };

//checking functions 58
static struct descriptor * find(const void *ptr) {
    struct descriptor *bp = htab[hash(ptr, htab)];

    while (bp && bp->ptr != ptr)
        bp = bp->link;
    return bp;
}

void Mem_free(void *ptr, const char *file, int line) {
    if (ptr) {
        struct descriptor *bp = find(ptr);
        if (((unsigned long)ptr) % (sizeof(union align)) != 0 || bp == NULL || bp->free)
            Except_raise(&Assert_Failed, file, line);
        bp->free = freelist.free;
        freelist.free = bp;
    }
}

void * Mem_resize(void *ptr, long nbytes, const char *file, int line) {
    assert(ptr);
    assert(nbytes > 0);

    struct descriptor *bp = find(ptr);
    if (((unsigned long)ptr) % (sizeof(union align)) != 0 || bp == NULL || bp->free)
        Except_raise(&Assert_Failed, file, line);
    void *newptr = Mem_alloc(nbytes, file, line);
    memcpy(newptr, ptr, nbytes < bp->size ? nbytes : bp->size);
    Mem_free(ptr, file, line);
    return newptr;
}

void * Mem_calloc(long count, long nbytes, const char *file, int line) {
    assert(count > 0);
    assert(nbytes > 0);

    void *ptr = Mem_alloc(count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);
    return ptr;
}

static struct descriptor * dalloc(void *ptr, long size, const char *file, int line) {
    static struct descriptor *avail;
    static int nleft;

    if (nleft <= 0) {
        avail = malloc(NDESCRIPTORS * sizeof(*avail));
        if (avail == NULL)
            return NULL;
        nleft = NDESCRIPTORS;
    }
    avail->ptr = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    avail->free = avail->link = NULL;
    --nleft;
    return avail++;
}

void * Mem_alloc(long nbytes, const char *file, int line) {
    assert(nbytes > 0);
    nbytes = ((nbytes + sizeof(union align) - 1) / (sizeof(union align))) * (sizeof(union align));
    for (struct descriptor *bp = freelist.free; bp; bp = bp->free) {
        if (bp->size > nbytes) {
            bp->size -= nbytes;
            void *ptr = (char *)bp->ptr + bp->size;
            if ((bp = dalloc(ptr, nbytes, file, line)) != NULL) {
                unsigned h = hash(ptr, htab);
                bp->link = htab[h];
                htab[h] = bp;
                return ptr;
            }
            else {
                if (file == NULL)
                    RAISE(Mem_Failed);
                else
                    Except_raise(&Mem_Failed, file, line);
            }
        }
        if (bp == &freelist) {
            void *ptr = malloc(nbytes + NALLOC);
            struct descriptor *newptr = dalloc(ptr, nbytes + NALLOC, __FILE__, __LINE__);
            if (ptr == NULL || newptr == NULL) {
                if (file == NULL)
                    RAISE(Mem_Failed);
                else
                    Except_raise(&Mem_Failed, file, line);
            }
            newptr->free = freelist.free;
            freelist.free = newptr;
        }
    }
    assert(0);
    return NULL;
}