#pragma once

typedef struct Array_T *Array_T;

//exported functions 117
Array_T Array_new(int length, int size);
void Array_free(Array_T *arraylist);
int Array_length(Array_T arraylist);
int Array_size(Array_T arraylist);
void * Array_get(Array_T arraylist, int i);
void * Array_put(Array_T arraylist, int i, void *elem);
void Array_resize(Array_T arraylist, int length);
Array_T Array_copy(Array_T arraylist, int length);