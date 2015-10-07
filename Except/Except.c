#include <stdlib.h>
#include <stdio.h>
#include "../Assert/Assert.h"
#include "Except.h"

#define T Except_T

Except_Frame * Except_Stack = NULL;

void Except_raise(const T * e, const char * file, int line) {
    Except_Frame * p = Except_stack;

    
}
#undef T