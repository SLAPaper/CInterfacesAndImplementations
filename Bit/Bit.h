#pragma once
#include <stdbool.h>

typedef struct Bit_T *Bit_T;

//exported functions 142
Bit_T Bit_new(int length);
int Bit_length(Bit_T set);
int Bit_count(Bit_T set);
void Bit_free(Bit_T *set);
int Bit_get(Bit_T set, int n);
int Bit_put(Bit_T set, int n, int bit);
void Bit_clear(Bit_T set, int lo, int hi);
void Bit_set(Bit_T set, int lo, int hi);
void Bit_not(Bit_T set, int lo, int hi);
bool Bit_lt(Bit_T s, Bit_T t);
bool Bit_eq(Bit_T s, Bit_T t);
bool Bit_leq(Bit_T s, Bit_T t);
void Bit_map(Bit_T set, void apply(int n, int bit, void *cl), void *cl);
Bit_T Bit_union(Bit_T s, Bit_T t);
Bit_T Bit_inter(Bit_T s, Bit_T t);
Bit_T Bit_minus(Bit_T s, Bit_T t);
Bit_T Bit_diff(Bit_T s, Bit_T t);