#include <limits.h>
#include <stddef.h>

#include "Mem\Mem.h"
#include "Assert\Assert.h"
#include "Arith\Arith.h"
#include "Set.h"

//types 108
struct Set_T {
    int length;
    unsigned int timestamp;
    int(*cmp)(const void *x, const void *y);
    unsigned int(*hash)(const void *x);
    int size;
    struct member {
        struct member *link;
        const void *member;
    } **buckets;
};

//static functions 108
static int cmpatom(const void *x, const void *y) {
    return x != y;
}

static unsigned int hashatom(const void *x) {
    return (uintptr_t)x >> 2 & UINT_MAX;
}

//functions 108
Set_T Set_new(int hint, int cmp(const void *x, const void *y), unsigned int hash(const void *x)) {
    static int primes[] = { 509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX };

    assert(hint >= 0);
    int i;
    for (i = 0; primes[i] < hint; ++i)
        ;
    Set_T set = ALLOC(sizeof(*set) + primes[i - 1] * sizeof(set->buckets[0]));
    set->size = primes[i - 1];
    set->cmp = cmp ? cmp : cmpatom;
    set->hash = hash ? hash : hashatom;
    set->buckets = (struct member **)(set + 1);
    for (i = 0; i < set->size; ++i) {
        set->buckets[i] = NULL;
    }
    set->length = 0;
    set->timestamp = 0;
    return set;
}

bool Set_member(Set_T set, const void *member) {
    assert(set);
    assert(member);

    struct member *p;
    //search set for member 109
    int i = set->hash(member) % set->size;
    for (p = set->buckets[i]; p; p = p->link) {
        if (set->cmp(member, p->member) == 0) {
            break;
        }
    }
    return p != NULL;
}