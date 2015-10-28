#pragma once

typedef struct Stack_T *Stack_T;

Stack_T Stack_new(void);
int Stack_empty(Stack_T stk);
void Stack_push(Stack_T stk, void *x);
void * Stack_pop(Stack_T stk);
void Stack_free(Stack_T *stk);