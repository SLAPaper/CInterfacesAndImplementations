#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Assert\Assert.h"
#include "Seq.h"
#include "Array\Array.h"
#include "Array\ArrayRep.h"
#include "Mem\Mem.h"

struct Seq_T {
    struct Array_T arraylist;
    int length;
    int head;
};

//static functions 128
static void expand(Seq_T seq) {
    int n = seq->arraylist.length;
    Array_resize(&seq->arraylist, 2 * n);
    if (seq->head > 0) {
        //slide tail down 129
        void **old = &((void **)seq->arraylist.arraylist)[seq->head];
        memcpy(old + n, old, (n - seq->head) * sizeof(void *));
        seq->head += n;
    }
}

//functions 126
Seq_T Seq_new(int hint) {
    assert(hint >= 0);

    Seq_T seq;
    NEW0(seq);
    if (hint == 0) {
        hint = 16;
    }
    ArrayRep_init(&seq->arraylist, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));
    return seq;
}

Seq_T Seq_seq(void *x, ...) {
    Seq_T seq = Seq_new(0);

    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        Seq_addHi(seq, x);
    }
    va_end(ap);
    return seq;
}

void Seq_free(Seq_T *seq) {
    assert(seq && *seq);
    assert((void *)*seq == (void *)&(*seq)->arraylist);
    Array_free((Array_T *)seq);
}

int Seq_length(Seq_T seq) {
    assert(seq);

    return seq->length;
}

void * Seq_get(Seq_T seq, int i) {
    assert(seq);
    assert(i >= 0 && i < seq->length);

    return ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
}

void * Seq_put(Seq_T seq, int i, void *x) {
    assert(seq);
    assert(i >= 0 && i < seq->length);

    void *prev = ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
    ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length] = x;
    return prev;
}

void * Seq_addHi(Seq_T seq, void *x) {
    assert(seq);

    if (seq->length == seq->arraylist.length) {
        expand(seq);
    }
    int i = seq->length++;
    return ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
}

void * Seq_addLo(Seq_T seq, void *x) {
    assert(seq);

    if (seq->length == seq->arraylist.length) {
        expand(seq);
    }
    if (seq->head - 1 < 0) {
        seq->head = seq->arraylist.length;
    }

    int i = 0;
    return ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
}

void * Seq_remHi(Seq_T seq) {
    assert(seq);
    assert(seq->length > 0);

    int i = --seq->length;
    return ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
}

void * Seq_remLow(Seq_T seq) {
    assert(seq);
    assert(seq->length > 0);

    int i = 0;
    void *x = ((void **)seq->arraylist.arraylist)[(seq->head + i) % seq->arraylist.length];
    seq->head = (seq->head + 1) % seq->arraylist.length;
    --seq->length;

    return x;
}