#pragma once

#define T Stack_T
typedef struct T *T;

T Stack_new(void);
int Stack_empty(T stk);
void Stack_push(T stk, void *x);
void * Stack_pop(T stk);
void Stack_free(T *stk);

#undef T