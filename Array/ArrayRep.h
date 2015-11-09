#pragma once

struct Array_T {
    int length;
    int size;
    char *arraylist;
};

void ArrayRep_init(Array_T arraylist, int length, int size, void *ary);