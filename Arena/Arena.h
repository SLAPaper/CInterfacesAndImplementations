#pragma once
#include "../Except/Except.h"

#define T Arena_T
typedef struct T *T;

const Except_T Arena_NewFailed;
const Except_T Arena_Failed;

//exported funtions 66
T Arena_new(void);
void Arena_dispose(T *ap);
void * Arena_alloc(T arena, long nbytes, const char *file, int line);
void * Arena_calloc(T arena, long count, long nbytes, const char *file, int line);
void Arena_free(T arena);

#undef T