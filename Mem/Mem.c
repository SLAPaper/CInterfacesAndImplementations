#ifndef CHKENABLED
#include <stdlib.h>
#include <stddef.h>
#include "../Assert/Assert.h"
#include "../Except/Except.h"
#include "Mem.h"

//data 54
const Except_T Mem_Failed = { "Allocation Failed" };

//functions 54
void * Mem_alloc(long nbytes, const char *file, int line) {
    assert(nbytes > 0);

    void *ptr = malloc(nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(Mem_Failed);
        else {
            Except_raise(&Mem_Failed, file, line);
        }
    }
    return ptr;
}

void * Mem_calloc(long count, long nbytes, const char *file, int line) {
    assert(count > 0);
    assert(nbytes > 0);

    void *ptr = calloc(count, nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(Mem_Failed);
        else {
            Except_raise(&Mem_Failed, file, line);
        }
    }
    return ptr;
}

void Mem_free(void *ptr, const char *file, int line) {
    if (ptr)
        free(ptr);
}

void * Mem_resize(void *ptr, long nbytes, const char *file, int line) {
    assert(ptr);
    assert(nbytes > 0);

    void *temp = realloc(ptr, nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(Mem_Failed);
        else {
            Except_raise(&Mem_Failed, file, line);
        }
    }
    ptr = temp;
    return ptr;
}

// excercise 5.4
FILE *log_file = NULL;
void Mem_log(FILE *log) {

}

// excercise 5.5
void Mem_leak(void apply(const void *ptr, long size, const char *file, int line, void *cl), void *cl) {

}
#endif