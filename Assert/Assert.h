#pragma once
#undef assert

#ifdef NDEBUG
#define assert(e) ((void) 0)
#else
#include "../Except/Except.h"
extern void assert(int e);
#define assert(e) ((void)((e) || (RAISE(Assert_Failed), 0)))
#endif