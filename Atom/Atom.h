#pragma once

int Atom_length(const char *str);
const char * Atom_new(const char *str, int len);
const char * Atom_string(const char *str);
const char * Atom_int(long n);

//exercise 3.8
void Atom_free(const char *str);
void Atom_reset(void);

//exercise 3.9
void Atom_vload(const char *str, ...);
void Atom_aload(const char *strs[]);

//exercise 3.10
const char * Atom_add(const char *str, int len);