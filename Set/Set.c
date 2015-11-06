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

static Set_T copy(Set_T t, int hint) {
    assert(t);
    Set_T set = Set_new(hint, t->cmp, t->hash);
    //for each member p in t 112
    for (int i = 0; i < t->size; ++i) {
        for (struct member *p = t->buckets[i]; p; p = p->link) {
            //add p->member to set 112
            const void *member = p->member;
            int j = set->hash(member) % set->size;
            //add member to set 109
            struct member *q;
            NEW(q);
            q->member = member;
            q->link = set->buckets[j];
            set->buckets[j] = q;
            ++set->length;
        }
    }
    return set;
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

void Set_put(Set_T set, const void *member) {
    assert(set);
    assert(member);

    //search set for member 109
    int i = set->hash(member) % set->size;
    struct member *p;
    for (p = set->buckets[i]; p; p = p->link) {
        if (set->cmp(member, p->member) == 0) {
            break;
        }
    }
    if (p == NULL) {
        //add member to set 109
        NEW(p);
        p->member = member;
        p->link = set->buckets[i];
        set->buckets[i] = p;
        ++set->length;
    }
    else {
        p->member = member;
        ++set->timestamp;
    }
}

void * Set_remove(Set_T set, const void *member) {
    assert(set);
    assert(member);
    ++set->timestamp;
    int i = set->hash(member) % set->size;
    for (struct member **pp = &set->buckets[i]; *pp; pp = &(*pp)->link)
    {
        if (set->cmp(member, (*pp)->member) == 0) {
            struct member *p = *pp;
            member = p->member;
            FREE(p);
            --set->length;
            return (void *)member;
        }
    }
    return NULL;
}

int Set_length(Set_T set) {
    assert(set);
    return set->length;
}

void Set_free(Set_T *set) {
    assert(set && *set);
    if ((*set)->length > 0) {
        for (int i = 0; i < (*set)->size; ++i) {
            for (struct member *p = (*set)->buckets[i], *q; p; p = q) {
                q = p->link;
                FREE(p);
            }
        }
    }
    FREE(*set);
}

void Set_map(Set_T set, void apply(const void *member, void *cl), void *cl) {
    assert(set);
    assert(apply);
    unsigned int stamp = set->timestamp;
    for (int i = 0; i < set->size; ++i) {
        for (struct member *p = set->buckets[i]; p; p = p->link) {
            apply(p->member, cl);
            assert(set->timestamp == stamp);
        }
    }
}

void ** Set_toArray(Set_T set, void *end) {
    assert(set);
    void **arraylist = ALLOC((set->length + 1) * sizeof(*arraylist));
    int j = 0;
    for (int i = 0; i < set->size; ++i) {
        for (struct member *p = set->buckets[i]; p; p = p->link) {
            arraylist[j++] = (void *)p->member;
        }
    }
    arraylist[j] = end;
    return arraylist;
}

Set_T Set_union(Set_T s, Set_T t) {
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    }
    else if (t == NULL) {
        return copy(s, s->size);
    }
    else {
        assert(s->cmp == t->cmp && s->hash == t->hash);
        Set_T set = copy(s, Arith_max(s->size, t->size));
        for (int i = 0; i < t->size; ++i) {
            for (struct member *p = t->buckets[i]; p; p = p->link) {
                Set_put(set, p->member);
            }
        }
        return set;
    }
}

Set_T Set_inter(Set_T s, Set_T t) {
    if (s == NULL) {
        assert(t);
        return Set_new(t->size, t->cmp, t->hash);
    }
    else if (t == NULL) {
        return Set_new(s->size, s->cmp, s->hash);
    }
    else if (s->length < t->length) {
        return Set_inter(t, s);
    }
    else {
        assert(s->cmp == t->cmp && s->hash == t->hash);
        Set_T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        //for each member p in t 112
        for (int i = 0; i < t->size; ++i) {
            for (struct member *p = t->buckets[i]; p; p = p->link) {
                if (Set_member(s, p->member)) {
                    //add p->member to set 112
                    const void *member = p->member;
                    int j = set->hash(member) % set->size;
                    //add member to set 109
                    struct member *q;
                    NEW(q);
                    q->member = member;
                    q->link = set->buckets[j];
                    set->buckets[j] = q;
                    ++set->length;
                }
            }
        }
        return set;
    }
}

Set_T Set_minus(Set_T t, Set_T s) {
    if (t == NULL) {
        assert(s);
        return Set_new(s->size, s->cmp, s->hash);
    }
    else if (s == NULL) {
        return copy(t, t->size);
    }
    else {
        assert(s->cmp == t->cmp && s->hash == t->hash);
        Set_T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        //for each member p in t 112
        for (int i = 0; i < t->size; ++i) {
            for (struct member *p = t->buckets[i]; p; p = p->link) {
                if (!Set_member(s, p->member)) {
                    //add p->member to set 112
                    const void *member = p->member;
                    int j = set->hash(member) % set->size;
                    //add member to set 109
                    struct member *q;
                    NEW(q);
                    q->member = member;
                    q->link = set->buckets[j];
                    set->buckets[j] = q;
                    ++set->length;
                }
            }
        }
        return set;
    }
}

Set_T Set_diff(Set_T s, Set_T t) {
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    }
    else if (t == NULL) {
        return copy(s, s->size);
    }
    else {
        assert(s->cmp == t->cmp && s->hash == t->hash);
        Set_T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        //for each member p in t 112
        for (int i = 0; i < t->size; ++i) {
            for (struct member *p = t->buckets[i]; p; p = p->link) {
                if (!Set_member(s, p->member)) {
                    //add p->member to set 112
                    const void *member = p->member;
                    int j = set->hash(member) % set->size;
                    //add member to set 109
                    struct member *q;
                    NEW(q);
                    q->member = member;
                    q->link = set->buckets[j];
                    set->buckets[j] = q;
                    ++set->length;
                }
            }
        }
        {
            Set_T u = t;
            t = s;
            s = u;
        }
        //for each member p in t 112
        for (int i = 0; i < t->size; ++i) {
            for (struct member *p = t->buckets[i]; p; p = p->link) {
                if (!Set_member(s, p->member)) {
                    //add p->member to set 112
                    const void *member = p->member;
                    int j = set->hash(member) % set->size;
                    //add member to set 109
                    struct member *q;
                    NEW(q);
                    q->member = member;
                    q->link = set->buckets[j];
                    set->buckets[j] = q;
                    ++set->length;
                }
            }
        }
        return set;
    }
}