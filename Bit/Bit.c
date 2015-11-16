#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "Assert\Assert.h"
#include "Mem\Mem.h"

#include "Bit.h"

struct Bit_T {
    int length;
    unsigned char *bytes;
    unsigned int *words;
};

//macros 145
#define BPW (8*sizeof(unsigned int))
#define nwords(len) ((((len) + BPW - 1) & (~(BPW - 1))) / BPW)
#define nbytes(len) ((((len) + 8 - 1) & (~(8 - 1))) / 8)
#define setop(sequal, snull, tnull, op) \
    if (s == t) {\
        assert(s);\
        return sequal;\
    }\
    else if (s == NULL) {\
        assert(t);\
        return snull;\
    }\
    else if (t == NULL) {\
        return tnull;\
    }\
    else {\
        assert(s->length == t->length);\
        Bit_T set = Bit_new(s->length);\
        for (int i = nwords(s->length) - 1; i >= 0; --i) {\
            set->words[i] = s->words[i] op t->words[i]; \
    }\
    return set;\
}

//static data 148
unsigned char msbmask[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
unsigned char lsbmask[] = { 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

//static functions 152
static Bit_T copy(Bit_T t) {
    assert(t);

    Bit_T set = Bit_new(t->length);
    if (t->length > 0) {
        memcpy(set->bytes, t->bytes, nbytes(t->length));
    }
    return set;
}

//functions 145
Bit_T Bit_new(int length) {
    assert(length >= 0);
    Bit_T set;
    NEW(set);
    if (length > 0) {
        set->words = CALLOC(nwords(length), sizeof(unsigned long));
    }
    else {
        set->words = NULL;
    }
    set->bytes = (unsigned char *)set->words;
    set->length = length;
    return set;
}

Bit_T Bit_bit(int length, ...) {
    va_list ap;
    va_start(ap, length);

    Bit_T set = Bit_new(length);
    bool x = va_arg(ap, bool);
    for (int i = 0; i < length; ++i, x = va_arg(ap, bool)) {
        if (x) {
            Bit_put(set, i, x);
        }
    }
    va_end(ap);
    return set;
}

void Bit_free(Bit_T *set) {
    assert(set && *set);
    FREE((*set)->words);
    FREE(*set);
}

int Bit_length(Bit_T set) {
    assert(set);
    return set->length;
}

int Bit_count(Bit_T set) {
    static char count[] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };

    assert(set);

    int length = 0;
    for (int n = nbytes(set->length) - 1; n >= 0; --n) {
        unsigned char c = set->bytes[n];
        length += count[c & 0xF] + count[c >> 4];
    }
    return length;
}

bool Bit_get(Bit_T set, int n) {
    assert(set);
    assert(0 <= n && n < set->length);
    return ((set->bytes[n / 8] >> (n % 8)) & 1);
}

bool Bit_put(Bit_T set, int n, bool bit) {
    assert(set);
    assert(bit == false || bit == true);
    assert(0 <= n && n < set->length);
    bool prev = ((set->bytes[n / 8] >> (n % 8)) & 1);
    if (bit) {
        set->bytes[n / 8] |= 1 << (n % 8);
    }
    else {
        set->bytes[n / 8] &= ~(1 << (n % 8));
    }
    return prev;
}

void Bit_set(Bit_T set, int lo, int hi) {
    //check set, lo, and hi 148
    assert(set);
    assert(0 <= lo && hi < set->length);
    assert(lo <= hi);

    if (lo / 8 < hi / 8) {
        //set the most significant bits in bytes lo/8 148
        set->bytes[lo / 8] |= msbmask[lo % 8];

        //set all the bits in bytes lo / 8 + .. hi / 8 - 1 148
        for (int i = lo / 8 + 1; i < hi / 8; ++i) {
            set->bytes[i] = 0xFF;
        }
        //set the least significant bits in byte hi / 8 148
        set->bytes[hi / 8] |= lsbmask[hi % 8];
    }
    else {
        //set bits lo % 8 .. hi % 8 in byte lo / 8 149
        set->bytes[lo / 8] |= (msbmask[lo % 8] & lsbmask[hi % 8]);
    }
}

void Bit_clear(Bit_T set, int lo, int hi) {
    //check set, lo, and hi 148
    assert(set);
    assert(0 <= lo && hi < set->length);
    assert(lo <= hi);

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] &= ~msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; ++i) {
            set->bytes[i] = 0;
        }
        set->bytes[hi / 8] &= ~lsbmask[hi % 8];
    }
    else {
        set->bytes[lo / 8] &= ~(msbmask[lo % 8] & lsbmask[hi % 8]);
    }
}

void Bit_not(Bit_T set, int lo, int hi) {
    //check set, lo, and hi 148
    assert(set);
    assert(0 <= lo && hi < set->length);
    assert(lo <= hi);

    if (lo / 8 < hi / 8) {
        set->bytes[lo / 8] ^= msbmask[lo % 8];
        for (int i = lo / 8 + 1; i < hi / 8; ++i) {
            set->bytes[i] ^= 0xFF;
        }
        set->bytes[hi / 8] ^= lsbmask[hi % 8];
    }
    else {
        set->bytes[lo / 8] ^= (msbmask[lo % 8] & lsbmask[hi % 8]);
    }
}

void Bit_map(Bit_T set, void apply(int n, bool bit, void *cl), void *cl) {
    assert(set);
    for (int n = 0; n < set->length; ++n) {
        apply(n, (set->bytes[n / 8] >> (n % 8) & 1), cl);
    }
}

bool Bit_eq(Bit_T s, Bit_T t) {
    assert(s && t);
    assert(s->length == t->length);

    for (int i = nwords(s->length) - 1; i >= 0; --i) {
        if (s->words[i] != t->words[i]) {
            return false;
        }
    }
    return true;
}

bool Bit_lt(Bit_T s, Bit_T t) {
    assert(s && t);
    assert(s->length == t->length);

    bool lt = false;
    for (int i = nwords(s->length) - 1; i >= 0; --i) {
        if ((s->words[i] & ~t->words[i]) != 0) {
            return false;
        }
        else if (s->words[i] != t->words[i]) {
            lt = true;
        }
    }
    return lt;
}

bool Bit_leq(Bit_T s, Bit_T t) {
    assert(s && t);
    assert(s->length == t->length);

    for (int i = nwords(s->length) - 1; i >= 0; --i) {
        if ((s->words[i] & ~t->words[i]) != 0) {
            return false;
        }
    }
    return true;
}

Bit_T Bit_union(Bit_T s, Bit_T t) {
    setop(copy(t), copy(t), copy(s), | );
}

Bit_T Bit_inter(Bit_T s, Bit_T t) {
    setop(copy(t), Bit_new(t->length), Bit_new(s->length), &);
}

Bit_T Bit_minus(Bit_T s, Bit_T t) {
    setop(Bit_new(s->length), Bit_new(t->length), copy(s), &~);
}

Bit_T Bit_diff(Bit_T s, Bit_T t) {
    setop(Bit_new(s->length), copy(t), copy(s), ^);
}