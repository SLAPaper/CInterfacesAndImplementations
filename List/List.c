#include <stdarg.h>
#include <stddef.h>
#include "../Assert/Assert.h"
#include "../Mem/Mem.h"
#include "List.h"

List_T List_push(List_T list, void *x) {
    List_T p;
    NEW(p);
    p->first = x;
    p->rest = list;
    return p;
}

List_T List_list(void *x, ...) {
    List_T list, *p = &list;

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

List_T List_append(List_T list, List_T tail) {
    List_T *p = &list;
    while (*p)
        p = &(*p)->rest;
    *p = tail;
    return list;
}

List_T List_copy(List_T list) {
    List_T head, *p = &head;
    for (; list; list = list->rest) {
        NEW(*p);
        (*p)->first = list->first;
        p = &(*p)->rest;
    }
    *p = NULL;
    return head;
}

List_T List_pop(List_T list, void **x) {
    if (list) {
        List_T head = list->rest;
        if (x)
            *x = list->first;
        FREE(list);
        return head;
    }
    else
        return list;
}

List_T List_reverse(List_T list) {
    List_T head = NULL, next;
    for (; list; list = next) {
        next = list->rest;
        list->rest = head;
        head = list;
    }
    return head;
}

int List_length(List_T list) {
    int n;
    for (n = 0; list; list = list->rest)
        ++n;
    return n;
}

void List_free(List_T *list) {
    assert(list);

    List_T next;
    for (; *list; *list = next) {
        next = (*list)->rest;
        FREE(*list);
    }
}

void List_map(List_T list, void apply(void **x, void *cl), void *cl) {
    assert(apply);

    for (; list; list = list->rest)
        apply(&list->first, cl);
}

void ** List_toArray(List_T list, void *end) {
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