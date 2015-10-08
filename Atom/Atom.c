#include "Atom.h"
#include <string.h>
#include "../Assert/Assert.h"
#include <limits.h>
#include "../Mem/Mem.h"
#include <stdarg.h>
#include <stdbool.h>

// macros 28
#define NELEMS(x) ((sizeof(x)) / (sizeof((x)[0])))

// data 26
static struct atom {
    bool isconst;
    struct atom *link;
    int len;
    char *str;
} *buckets[2039];

static unsigned long scatter[] = { 40961, 18948, 51717, 36358, 43013, 42503, 50697, 36874, 32777, 14345, 37903, 55313, 14865, 18451, 21013, 60952, 33820, 45597, 8222, 34334, 12832, 7713, 2594, 58401, 5668, 27676, 4135, 60973, 46125, 55855, 50226, 36407, 41528, 62009, 45626, 57915, 22589, 11326, 49215, 55875, 41540, 52806, 43079, 4169, 23626, 3658, 7246, 38994, 58452, 3670, 48726, 11863, 14423, 37474, 38499, 49765, 45159, 64107, 18028, 30830, 27760, 17009, 22642, 24177, 32372, 40057, 26746, 11900, 15998, 4734, 45185, 60033, 44676, 14470, 63623, 39561, 32909, 35982, 55442, 146, 4754, 26771, 62614, 45718, 61592, 1686, 63131, 65180, 64668, 31393, 55970, 2723, 51362, 10917, 40105, 32426, 46763, 42667, 63662, 2736, 27825, 57522, 40628, 51893, 30902, 32952, 31417, 35514, 59582, 29889, 16580, 42182, 19656, 7880, 55498, 61134, 46287, 32463, 10447, 59605, 10966, 54488, 51929, 59098, 33502, 15582, 30944, 31971, 48355, 34533, 20710, 28397, 24302, 35057, 48885, 29943, 40697, 62202, 43771, 39167, 255, 36611, 19207, 50957, 271, 62224, 30480, 15644, 61724, 34080, 22818, 53031, 11051, 58162, 28467, 55605, 1846, 11575, 48439, 11573, 18747, 39231, 16194, 46403, 51011, 54597, 61766, 9541, 19781, 36180, 19799, 16728, 60249, 58203, 20318, 62815, 1378, 53607, 36714, 15211, 2926, 7024, 34161, 63347, 29044, 31093, 18805, 34174, 28030, 13695, 16256, 62337, 29567, 60294, 63369, 4491, 62859, 41362, 18324, 55189, 46486, 7070, 26015, 56222, 19875, 23977, 39851, 62891, 59309, 6574, 57771, 33200, 9649, 43953, 37294, 24501, 26039, 33209, 41916, 60860, 25022, 35262, 14787, 23492, 24005, 64454, 16839, 9161, 14795, 55756, 12749, 23505, 51666, 43988, 54231, 5080, 3546, 19418, 20957, 480, 25569, 7140, 15848, 42984, 46569, 65002, 59373, 5614, 45040, 6641, 60406, 43511, 31224, 7674, 19965, 17918 };

//functions 25
const char * Atom_string(const char *str) {
    assert(str);
    return Atom_new(str, (int)strlen(str));
}

const char * Atom_int(long n) {
    char str[43];
    char *s = str + sizeof(str);

    unsigned long m;
    if (n == LONG_MIN)
        m = LONG_MAX + 1UL;
    else if (n < 0)
        m = -n;
    else
        m = n;

    do
        *--s = m % 10 + '0';
    while ((m /= 10) > 0);

    if (n < 0)
        *--s = '-';

    return Atom_new(s, (int)((str + sizeof(str)) - s));
}

const char * Atom_new(const char *str, int len) {
    assert(str);
    assert(len >= 0);

    unsigned long h;
    int i;
    for (h = 0, i = 0; i < len; ++i)
        h = (h << 1) + scatter[(unsigned char)str[i]];
    h %= NELEMS(buckets);

    struct atom *p;
    for (p = buckets[h]; p; p = p->link)
        if (len == p->len) {
            for (i = 0; i < len && p->str[i] == str[i]; ++i)
                ;
            if (i == len)
                return p->str;
        }
    p = ALLOC(sizeof(*p) + len + 1);
    p->len = len;
    p->str = (char *)(p + 1);

    if (len > 0)
        memcpy(p->str, str, len);

    p->str[len] = '\0';
    p->isconst = false;

    p->link = buckets[h];
    buckets[h] = p;

    return p->str;
}

int Atom_length(const char *str) {
    assert(str);
    int len = (int)strlen(str);

    int i;
    unsigned long h;
    for (h = 0, i = 0; i < len; ++i)
        h = (h << 1) + scatter[(unsigned char)str[i]];
    h %= NELEMS(buckets);

    for (struct atom *p = buckets[h]; p; p = p->link)
        if (p->str == str)
            return p->len;

    assert(0);
    return 0;
}

//exercise 3.8
extern void Atom_free(const char *str) {
    assert(str);
    int len = (int)strlen(str);

    int i;
    unsigned long h;
    for (h = 0, i = 0; i < len; ++i)
        h = (h << 1) + scatter[(unsigned char)str[i]];
    h %= NELEMS(buckets);

    for (struct atom *p = buckets[h]; p; p = p->link)
        if (p->str == str) {
            if (!(p->isconst))
                FREE(p->str);
            FREE(p);
        }
}

extern void Atom_reset(void) {
    for (int i = 0; i < NELEMS(buckets); ++i) {
        for (struct atom *p = buckets[i]; p; p->link) {
            if (!(p->isconst))
                FREE(p->str);
            FREE(p);
        }
    }
}

//exercise 3.9
extern void Atom_vload(const char *str, ...) {
    va_list args;
    char *arg;
    assert(str);

    Atom_string(str);

    va_start(args, str);
    while ((arg = va_arg(args, char *)) != NULL) {
        Atom_string(arg);
    }

    va_end(args);
}

extern void Atom_aload(const char *strs[]) {
    assert(strs);
    for (char * str = (char *)*strs; str; ++str) {
        Atom_string(str);
    }
}

//exercise 3.10
extern const char * Atom_add(const char *str, int len) {
    assert(str);
    assert(len >= 0);

    unsigned long h;
    int i;
    for (h = 0, i = 0; i < len; ++i)
        h = (h << 1) + scatter[(unsigned char)str[i]];
    h %= NELEMS(buckets);

    struct atom *p;
    for (p = buckets[h]; p; p = p->link)
        if (len == p->len) {
            for (i = 0; i < len && p->str[i] == str[i]; ++i)
                ;
            if (i == len)
                return p->str;
        }
    p = ALLOC(sizeof(*p));
    p->len = len;
    p->str = (char *)str;
    p->isconst = true;

    p->link = buckets[h];
    buckets[h] = p;

    return p->str;
}