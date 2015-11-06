//xref includes 103
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Table\Table.h"
#include "Atom\Atom.h"
#include "Set\Set.h"
#include "Mem\Mem.h"
#include "getword.h"

//xref prototypes 104
static int xref_compare(const void *x, const void *y);
static void xref_print(Table_T files);
static int xref_cmpint(const void *x, const void *y);
static void xref(const char *name, FILE *fp, Table_T identifiers);
static int xref_first(int c);
static int xref_rest(int c);
static int xref_intcmp(const void *x, const void *y);
static unsigned int xref_inthash(const void *x);

//xref data 105
int xref_linenum;

//xref functions 102
int xref_main(int argc, char *argv[]) {
    Table_T identifiers = Table_new(0, NULL, NULL);

    for (int i = 0; i < argc; ++i) {
        FILE *fp;
        fopen_s(&fp, argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: can't open '%s'\n", argv[0], argv[i]);
            return EXIT_FAILURE;
        }
        else {
            xref(argv[i], fp, identifiers);
            fclose(fp);
        }
    }
    if (argc == 1) {
        xref(NULL, stdin, identifiers);
    }
    //print the identifiers 103
    void **arraylist = Table_toArray(identifiers, NULL);
    qsort(arraylist, Table_length(identifiers), 2 * sizeof(*arraylist), xref_compare);
    for (int i = 0; arraylist[i]; i += 2) {
        printf("%s", (char *)arraylist[i]);
        xref_print(arraylist[i + 1]);
    }
    FREE(arraylist);
    return EXIT_SUCCESS;
}

static int xref_compare(const void *x, const void *y) {
    return strcmp(*(char **)x, *(char **)y);
}

static void xref_print(Table_T files) {
    void **arraylist = Table_toArray(files, NULL);
    qsort(arraylist, Table_length(files), 2 * sizeof(*arraylist), xref_compare);
    for (int i = 0; arraylist[i]; i += 2) {
        if (*(char *)arraylist[i] != '\0') {
            printf("\t%s:", (char *)arraylist[i]);
        }
        //print the line numbers in the set arraylist[i+1] 104
        void **lines = Set_toArray(arraylist[i + 1], NULL);
        qsort(lines, Set_length(arraylist[i + 1]), sizeof(*lines), xref_cmpint);
        for (int j = 0; lines[j]; ++j) {
            printf(" %d", *(int *)lines[j]);
        }
        FREE(lines);
        printf("\n");
    }
    FREE(arraylist);
}

static int xref_cmpint(const void *x, const void *y) {
    if (**(int **)x < **(int **)y)
        return -1;
    else if (**(int **)x > **(int **)y)
        return 1;
    else
        return 0;
}

static void xref(const char *name, FILE *fp, Table_T identifiers) {
    char buf[128];

    if (name == NULL) {
        name = "";
    }
    name = Atom_string(name);
    xref_linenum = 1;
    while (getword(fp, buf, sizeof(buf), xref_first, xref_rest)) {
        const char *id = Atom_string(buf);
        //files file table in identifiers associated with id 106
        Table_T files = Table_get(identifiers, id);
        if (files == NULL) {
            files = Table_new(0, NULL, NULL);
            Table_put(identifiers, id, files);
        }
        //set set in files associated with name 106
        Set_T set = Table_get(files, name);
        if (set == NULL) {
            set = Set_new(0, xref_intcmp, xref_inthash);
            Table_put(files, name, set);
        }
        //add xref_linenum to set, if necessary 107
        int *p = &xref_linenum;
        if (!Set_member(set, p)) {
            NEW(p);
            *p = xref_linenum;
            Set_put(set, p);
        }
    }
}

static int xref_first(int c) {
    if (c == '\n') {
        ++xref_linenum;
    }
    return isalpha(c) || c == '_';
}

static int xref_rest(int c) {
    return isalpha(c) || c == '_' || isdigit(c);
}

static int xref_intcmp(const void *x, const void *y) {
    return xref_cmpint(&x, &y);
}

static unsigned int xref_inthash(const void *x) {
    return *(int *)x;
}