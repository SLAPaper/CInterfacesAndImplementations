#pragma once

typedef struct Table_T *Table_T ;

//exported functions 84
Table_T Table_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *key));
void Table_free(Table_T *table);
int Table_length(Table_T table);
void * Table_put(Table_T table, const void *key, void *value);
void * Table_get(Table_T table, const void *key);
void * Table_remove(Table_T table, const void *key);
void Table_map(Table_T table, void apply(const void *key, void **value, void *cl), void *cl);
void ** Table_toArray(Table_T table, void *end);

#undef T