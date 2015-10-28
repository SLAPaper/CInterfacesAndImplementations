#pragma once
#include "../Except/Except.h"

typedef struct Arena_T *Arena_T;

const Except_T Arena_NewFailed;
const Except_T Arena_Failed;

//exported funtions 66
Arena_T Arena_new(void);
void Arena_dispose(Arena_T *ap);
void * Arena_alloc(Arena_T arena, long nbytes, const char *file, int line);
void * Arena_calloc(Arena_T arena, long count, long nbytes, const char *file, int line);
void Arena_free(Arena_T arena);