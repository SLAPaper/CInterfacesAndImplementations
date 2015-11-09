#include <stdlib.h>
#include <string.h>

#include "Assert\Assert.h"
#include "Mem\Mem.h"

#include "Array.h"
#include "ArrayRep.h"

//functions 120
Array_T Array_new(int length, int size) {
    Array_T arraylist;

    NEW(arraylist);
    if (length > 0) {
        ArrayRep_init(arraylist, length, size, CALLOC(length, size));
    }
    else {
        ArrayRep_init(arraylist, length, size, NULL);
    }
    return arraylist;
}

void ArrayRep_init(Array_T arraylist, int length, int size, void *ary) {
    assert(arraylist);
    assert(ary && length > 0 || length == 0 && ary == NULL);
    assert(size > 0);

    arraylist->length = length;
    arraylist->size = size;
    if (length > 0) {
        arraylist->arraylist = ary;
    }
    else {
        arraylist->arraylist = NULL;
    }
}

void Array_free(Array_T *arraylist) {
    assert(arraylist && *arraylist);

    FREE((*arraylist)->arraylist);
    FREE(*arraylist);
}

void * Array_get(Array_T arraylist, int i) {
    assert(arraylist);
    assert(i >= 0 && i < arraylist->length);

    return arraylist->arraylist + i * arraylist->size;
}

void * Array_put(Array_T arraylist, int i, void *elem) {
    assert(arraylist);
    assert(i >= 0 && i < arraylist->length);
    assert(elem);

    memcpy(arraylist->arraylist + i * arraylist->size, elem, arraylist->size);
    return elem;
}

int Array_length(Array_T arraylist) {
    assert(arraylist);

    return arraylist->length;
}

int Array_size(Array_T arraylist) {
    assert(arraylist);

    return arraylist->size;
}

void Array_resize(Array_T arraylist, int length) {
    assert(arraylist);
    assert(length >= 0);

    if (length == 0) {
        FREE(arraylist->arraylist);
    }
    else if (arraylist->length == 0) {
        arraylist->arraylist = ALLOC(length * arraylist->size);
    }
    else {
        RESIZE(arraylist->arraylist, length * arraylist->size);
    }
    arraylist->length = length;
}

Array_T Array_copy(Array_T arraylist, int length) {
    assert(arraylist);
    assert(length >= 0);

    Array_T copy = Array_new(length, arraylist->size);
    if (copy->length >= arraylist->length && arraylist->length > 0) {
        memcpy(copy->arraylist, arraylist->arraylist, arraylist->length * arraylist->size);
    }
    else if (arraylist->length > copy->length && copy->length > 0) {
        memcpy(copy->arraylist, arraylist->arraylist, copy->length * arraylist->size);
    }
    return copy;
}