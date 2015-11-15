#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "Assert\Assert.h"
#include "Mem\Mem.h"

#include "Ring.h"

struct node {
    struct node *llink;
    struct node *rlink;
    void *value;
};

struct Ring_T {
    struct node *head;
    int length;
};

//functions 134
Ring_T Ring_new(void) {
    Ring_T ring;
    NEW0(ring);
    ring->head = NULL;
    return ring;
}

Ring_T Ring_ring(void *x, ...) {
    Ring_T ring = Ring_new();
    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        Ring_addHi(ring, x);
    }
    va_end(ap);
    return ring;
}

void Ring_free(Ring_T *ring) {
    assert(ring && *ring);

    struct node *p = (*ring)->head;
    if (p != NULL) {
        struct node *ptemp = p;
        do {
            struct node *q = p->rlink;
            FREE(p);
            p = q;
        } while (p != ptemp);
    }
    FREE(*ring);
}

int Ring_length(Ring_T ring) {
    assert(ring);
    return ring->length;
}

void * Ring_get(Ring_T ring, int i) {
    assert(ring);
    assert(i >= 0 && i < ring->length);

    //q ith node 136
    struct node *q = ring->head;
    if (i <= ring->length / 2) {
        for (int n = i; n > 0; --n) {
            q = q->rlink;
        }
    }
    else {
        for (int n = i; n > 0; --n) {
            q = q->llink;
        }
    }
    return q->value;
}

void * Ring_put(Ring_T ring, int i, void *x) {
    assert(ring);
    assert(i >= 0 && i < ring->length);

    //q ith node 136
    struct node *q = ring->head;
    if (i <= ring->length / 2) {
        for (int n = i; n > 0; --n) {
            q = q->rlink;
        }
    }
    else {
        for (int n = i; n > 0; --n) {
            q = q->llink;
        }
    }
    void *prev = q->value;
    q->value = x;
    return prev;
}

void * Ring_addHi(Ring_T ring, void *x) {
    assert(ring);
    struct node *p;
    NEW(p);
    struct node *q = ring->head;
    if (q != NULL) {
        //insert p to the left of q 137
        p->llink = q->llink;
        q->llink->rlink = p;
        p->rlink = q;
        q->llink = p;
    }
    else {
        //make p ring's only value 137
        ring->head = p->llink = p->rlink = p;
    }
    ++ring->length;
    p->value = x;
    return x;
}

void * Ring_addLo(Ring_T ring, void *x) {
    assert(ring);
    Ring_addHi(ring, x);
    ring->head = ring->head->llink;
    return x;
}

void * Ring_add(Ring_T ring, int pos, void *x) {
    assert(ring);
    assert(pos >= -ring->length && pos <= ring->length + 1);
    if (pos == 1 || pos == -ring->length) {
        return Ring_addLo(ring, x);
    }
    else if (pos == 0 || pos == ring->length + 1) {
        return Ring_addHi(ring, x);
    }
    else {
        int i = pos < 0 ? pos + ring->length : pos - 1;

        //q ith node 136
        struct node *q = ring->head;
        if (i <= ring->length / 2) {
            for (int n = i; n > 0; --n) {
                q = q->rlink;
            }
        }
        else {
            for (int n = i; n > 0; --n) {
                q = q->llink;
            }
        }

        struct node *p;
        NEW(p);
        //insert p to the left of q 137
        p->llink = q->llink;
        q->llink->rlink = p;
        p->rlink = q;
        q->llink = p;

        ++ring->length;
        p->value = x;
        return x;
    }
}

void * Ring_remove(Ring_T ring, int i) {
    assert(ring);
    assert(ring->length > 0);
    assert(i >= 0 && i < ring->length);

    //q ith node 136
    struct node *q = ring->head;
    if (i <= ring->length / 2) {
        for (int n = i; n > 0; --n) {
            q = q->rlink;
        }
    }
    else {
        for (int n = i; n > 0; --n) {
            q = q->llink;
        }
    }

    if (i == 0) {
        ring->head = ring->head->rlink;
    }
    void *x = q->value;
    //delete node q 139
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    FREE(q);
    --ring->length;
    if (ring->length == 0) {
        ring->head = NULL;
    }

    return x;
}


void *Ring_remHi(Ring_T ring) {
    assert(ring);
    assert(ring->length > 0);

    struct node *q = ring->head->llink;
    void *x = q->value;
    //delete node q 139
    q->llink->rlink = q->rlink;
    q->rlink->llink = q->llink;
    FREE(q);
    --ring->length;
    if (ring->length == 0) {
        ring->head = NULL;
    }

    return x;
}

void *Ring_remLo(Ring_T ring) {
    assert(ring);
    assert(ring->length > 0);

    ring->head = ring->head->rlink;
    return Ring_remHi(ring);
}

void Ring_rotate(Ring_T ring, int n) {
    assert(ring);
    assert(n >= -ring->length && n <= ring->length);

    int i;
    if (n >= 0) {
        i = n % ring->length;
    }
    else {
        i = n + ring->length;
    }
    
    //q ith node 136
    struct node *q = ring->head;
    if (i <= ring->length / 2) {
        for (int n = i; n > 0; --n) {
            q = q->rlink;
        }
    }
    else {
        for (int n = i; n > 0; --n) {
            q = q->llink;
        }
    }
    ring->head = q;
}