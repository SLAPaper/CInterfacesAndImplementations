#include <limits.h>
#include <stddef.h>
#include "../Mem/Mem.h"
#include "../Assert/Assert.h"
#include "Table.h"

//types 92
struct Table_T {
    //fields 92
    int size;
    int(*cmp)(const void *x, const void *y);
    unsigned(*hash)(const void *key);
    
    struct binding {
        struct binding *link;
        const void *key;
        void *value;
    } **buckets;
};

//static functions 93

//functions 92