#pragma once

extern int Atom_length(const char *str);
extern const char * Atom_new(const char *str, int len);
extern const char * Atom_string(const char *str);
extern const char * Atom_int(long n);

//exercise 3.8
extern void Atom_free(const char *str);
extern void Atom_reset(void);

//exercise 3.9
extern void Atom_vload(const char *str, ...);
extern void Atom_aload(const char *strs[]);

//exercise 3.10
extern const char * Atom_add(const char *str, int len);