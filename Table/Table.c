#include <limits.h>
#include <stddef.h>
#include "../Mem/Mem.h"
#include "../Assert/Assert.h"
#include "Table.h"

//types 92
struct Table_T {
    //fields 92
    int size;
    int(*cmp)(const void *x, const void *y);
    unsigned(*hash)(const void *key);
    int length;
    unsigned int timestamp;

    struct binding {
        struct binding *link;
        const void *key;
        void *value;
    } **buckets;
};

//static functions 93
static int cmpatom(const void *x, const void *y) {
    return x != y;
}

static unsigned int hashatom(const void *key) {
    return ((uintptr_t)key >> 2) & UINT_MAX;
}

//functions 92
Table_T Table_new(int hint, int cmp(const void *x, const void *y), unsigned int hash(const void *key)) {
    static int primes[] = { 509, 509,1021,2053,4093,8191,16381,32771,65521,INT_MAX };

    assert(hint >= 0);

    int i = 1;
    while (primes[i] < hint) {
        ++i;
    }
    Table_T table = ALLOC(sizeof(*table) + primes[i - 1] * sizeof(table->buckets[0]));
    table->size = primes[i - 1];
    table->cmp = cmp ? cmp : cmpatom;
    table->hash = hash ? hash : hashatom;
    table->buckets = (struct binding **)(table + 1);
    for (int i = 0; i < table->size; ++i) {
        table->buckets[i] = NULL;
    }
    table->length = 0;
    table->timestamp = 0;
    return table;
}

void * Table_get(Table_T table, const void *key) {
    assert(table);
    assert(key);

    int i = table->hash(key) % table->size;
    struct binding *p;
    for (p = table->buckets[i]; p; p = p->link) {
        if (table->cmp(key, p->key) == 0)
            break;
    }
    return p ? p->value : NULL;
}