//wf includes 88
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include "Atom\Atom.h"
#include "Table\Table.h"
#include "Mem\Mem.h"

#include "getword.h"

//wf prototypes 90
static void wf(char *name, FILE *fp);
static int wf_first(int c);
static int wf_rest(int c);
static int wf_compare(const void *x, const void *y);
static void wf_vfree(const void *key, void **count, void *cl);

//wf functions 88
int wf_main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        FILE *fp;
        fopen_s(&fp, argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: can't open '%s'\n", argv[0], argv[i]);
            return EXIT_FAILURE;
        }
        else {
            wf(argv[i], fp);
            fclose(fp);
        }
    }
    if (argc == 1) {
        wf(NULL, stdin);
    }
    return EXIT_SUCCESS;
}

static int wf_first(int c) {
    return isalpha(c);
}

static int wf_rest(int c) {
    return isalpha(c) || c == '_';
}

static int wf_compare(const void *x, const void *y) {
    return strcmp(*(char **)x, *(char **)y);
}

static void wf(char *name, FILE *fp) {
    Table_T table = Table_new(0, NULL, NULL);
    char buf[128];

    while (getword(fp, buf, sizeof(buf), wf_first, wf_rest)) {
        for (int i = 0; buf[i] != '\0'; ++i) {
            buf[i] = tolower(buf[i]);
        }
        const char *word = Atom_string(buf);
        int *count = Table_get(table, word);
        if (count) {
            ++(*count);
        }
        else {
            NEW(count);
            *count = 1;
            Table_put(table, word, count);
        }
    }
    if (name) {
        printf("%s:\n", name);
    }
    // print the words 90
    void **arraylist = Table_toArray(table, NULL);
    qsort(arraylist, Table_length(table), 2 * sizeof(*arraylist), wf_compare);
    for (int i = 0; arraylist[i]; i += 2) {
        printf("%d\t%s\n", *(int *)arraylist[i + 1], (char *)arraylist[i]);
    }
    FREE(arraylist);

    // deallocate the entries and table 91
    Table_map(table, wf_vfree, NULL);
    Table_free(&table);
}

static void wf_vfree(const void *key, void **count, void *cl) {
    FREE(*count);
}