#include <stdarg.h>
#include <stddef.h>
#include "../Assert/Assert.h"
#include "../Mem/Mem.h"
#include "List.h"

#define T List_T

T List_push(T list, void *x) {
    T p;
    NEW(p);
    p->first = x;
    p->rest = list;
    return p;
}

T List_list(void *x, ...) {
    T list, *p = &list;

    va_list ap;
    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        NEW(*p);
        (*p)->first = x;
        p = &(*p)->rest;
    }
    *p = NULL;
    va_end(ap);
    return list;
}

T List_append(T list, T tail) {
    T *p = &list;
    while (*p)
        p = &(*p)->rest;
    *p = tail;
    return list;
}

T List_copy(T list) {
    T head, *p = &head;
    for (; list; list = list->rest) {
        NEW(*p);
        (*p)->first = list->first;
        p = &(*p)->rest;
    }
    *p = NULL;
    return head;
}

T List_pop(T list, void **x) {
    if (list) {
        T head = list->rest;
        if (x)
            *x = list->first;
        FREE(list);
        return head;
    }
    else
        return list;
}

T List_reverse(T list) {
    T head = NULL, next;
    for (; list; list = next) {
        next = list->rest;
        list->rest = head;
        head = list;
    }
    return head;
}

int List_length(T list) {
    int n;
    for (n = 0; list; list = list->rest)
        ++n;
    return n;
}

void List_free(T *list) {
    assert(list);

    T next;
    for (; *list; *list = next) {
        next = (*list)->rest;
        FREE(*list);
    }
}

void List_map(T list, void apply(void **x, void *cl), void *cl) {
    assert(apply);

    for (; list; list = list->rest)
        apply(&list->first, cl);
}

void ** List_toArray(T list, void *end) {
    int n = List_length(list);
    void ** arraylist = ALLOC((n + 1) * sizeof(*arraylist));

    int i;
    for (i = 0; i < n; ++i) {
        arraylist[i] = list->first;
        list = list->rest;
    }
    arraylist[i] = end;
    return arraylist;
}
#undef T