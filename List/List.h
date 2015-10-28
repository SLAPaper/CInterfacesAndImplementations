#pragma once

typedef struct List_T *List_T;

struct List_T {
    struct List_T *rest;
    void *first;
};

List_T List_append(List_T list, List_T tail);
List_T List_copy(List_T list);
List_T List_list(void *x, ...);
List_T List_pop(List_T list, void **x);
List_T List_push(List_T list, void *x);
List_T List_reverse(List_T list);
int List_length(List_T list);
void List_free(List_T *list);
void List_map(List_T list, void apply(void **x, void *cl), void *cl);
void ** List_toArray(List_T list, void *end);