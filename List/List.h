#pragma once

#define T List_T
typedef struct T *T;

struct T {
    T rest;
    void *first;
};

T List_append(T list, T tail);
T List_copy(T list);
T List_list(void *x, ...);
T List_pop(T list, void **x);
T List_push(T list, void *x);
T List_reverse(T list);
int List_length(T list);
void List_free(T *list);
void List_map(T list, void apply(void **x, void *cl), void *cl);
void ** List_toArray(T list, void *end);

#undef T