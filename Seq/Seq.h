#pragma once

typedef struct Seq_T *Seq_T;

//exported functions 123
Seq_T Seq_new(int hint);
Seq_T Seq_seq(void *x, ...);
void Seq_free(Seq_T *seq);
int Seq_length(Seq_T seq);
void * Seq_get(Seq_T seq, int i);
void * Seq_put(Seq_T seq, int i, void *x);
void * Seq_addLo(Seq_T seq, void *x);
void * Seq_addHi(Seq_T seq, void *x);
void * Seq_remLo(Seq_T seq);
void * Seq_remHi(Seq_T seq);