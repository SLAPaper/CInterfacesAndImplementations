#pragma once
typedef struct Ring_T *Ring_T;

//exported functions 131
Ring_T Ring_new(void);
Ring_T Ring_ring(void *x, ...);
void Ring_free(Ring_T *ring);
int Ring_length(Ring_T ring);
void * Ring_get(Ring_T ring, int i);
void * Ring_put(Ring_T ring, int i, void *x);
void * Ring_add(Ring_T ring, int pos, void *x);
void * Ring_addLo(Ring_T ring, void *x);
void * Ring_addHi(Ring_T ring, void *x);
void * Ring_remove(Ring_T ring, int i);
void * Ring_removeLo(Ring_T ring);
void * Ring_removeHi(Ring_T ring);
void Ring_rotate(Ring_T ring, int n);