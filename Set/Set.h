#pragma once
#include <stdbool.h>

typedef struct Set_T *Set_T;

//exported functions 100
Set_T Set_new(int hint, int cmp(const void *x, const void *y), unsigned int hash(const void *x));
void Set_free(Set_T *set);
int Set_length(Set_T set);
bool Set_member(Set_T set, const void *member);
void Set_put(Set_T set, const void *member);
void * Set_remove(Set_T set, const void *member);
void Set_map(Set_T set, void apply(const void *member, void *cl), void *cl);
void ** Set_toArray(Set_T set, void *end);
Set_T Set_union(Set_T s, Set_T t);
Set_T Set_inter(Set_T s, Set_T t);
Set_T Set_minus(Set_T s, Set_T t);
Set_T Set_diff(Set_T s, Set_T t);