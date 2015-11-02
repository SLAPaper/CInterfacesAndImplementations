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

void * Table_put(Table_T table, const void *key, void *value) {
    assert(table);
    assert(key);

    int i = table->hash(key) % table->size;
    struct binding *p;
    void *prev = NULL;
    for (p = table->buckets[i]; p; p = p->link) {
        if (table->cmp(key, p->key) == 0)
            break;
    }
    if (p == NULL) {
        NEW(p);
        p->key = key;
        p->link = table->buckets[i];
        table->buckets[i] = p;
        ++table->length;
        prev = NULL;
    }
    else {
        prev = p->value;
    }
    ++table->timestamp;
    return prev;
}

int Table_length(Table_T table) {
    assert(table);
    return table->length;
}

void Table_map(Table_T table, void apply(const void *key, void **value, void *cl), void *cl) {
    assert(table);
    assert(apply);
    unsigned int stamp = table->timestamp;
    for (int i = 0; i < table->size; ++i) {
        for (struct binding *p = table->buckets[i]; p; p = p->link) {
            apply(p->key, p->value, cl);
            assert(table->timestamp = stamp);
        }
    }
}

void * Table_remove(Table_T table, const void *key) {
    assert(table);
    assert(key);

    ++table->timestamp;
    int i = table->hash(key) % table->size;
    for (struct binding **pp = &table->buckets[i]; *pp; pp = &(*pp)->link) {
        if (table->cmp(key, (*pp)->key) == 0) {
            struct binding *p = *pp;
            void *value = p->value;
            *pp = p->link;
            FREE(p);
            --table->length;
            return value;
        }
    }
    return NULL;
}

void ** Table_toArray(Table_T table, void *end) {
    assert(table);

    void **arraylist = ALLOC((2 * table->length + 1) * sizeof(*arraylist));
    int j = 0;
    for (int i = 0; i < table->size; ++i) {
        for (struct binding *p = table->buckets[i]; p; p = p->link) {
            arraylist[j++] = (void *)p->key;
            arraylist[j++] = p->value;
        }
    }
    arraylist[j] = end;
    return arraylist;
}

void Table_free(Table_T *table) {
    assert(table && *table);
    if ((*table)->length > 0) {
        for (int i = 0; i < (*table)->size; ++i) {
            for (struct binding *p = (*table)->buckets[i], *q = NULL; p; p = q) {
                q = p->link;
                FREE(p);
            }
        }
    }
    FREE(*table);
}