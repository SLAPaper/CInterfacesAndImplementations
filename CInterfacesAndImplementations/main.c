#include <stdio.h>
#include <stdlib.h>

#include "Arena\Arena.h"
#include "Arith\Arith.h"
#include "Assert\Assert.h"
#include "Atom\Atom.h"
#include "Except\Except.h"
#include "List\List.h"
#include "Mem\Mem.h"
#include "Stack\Stack.h"
#include "Table\Table.h"
#include "Set\Set.h"
#include "Array\Array.h"
#include "Seq\Seq.h"
#include "Ring\Ring.h"
#include "Bit\Bit.h"

#include "examples.h"

#define ASSUME_EQUAL(a, b) assert(a == b)
#define ASSUME_SAME(a, b) assert(&a == &b)

void test_Arith() {

}

void test_Atom() {

}

void test_Except() {

}

void test_Assert() {

}

void test_Mem() {

}

void test_List() {

}

void test_Table() {

}

void test_Set() {

}

void test_Array() {

}

void test_Seq() {

}

void test_Ring() {

}

void test_Bit() {

}

int wf_main(int argc, char *argv[]);

int main(int argc, char* argv[]) {
    printf("Hello World!\n");
    printf("ÄãºÃ£¬ÊÀ½ç£¡\n");
    printf("%llu %llu %llu\n", sizeof(unsigned int), sizeof(unsigned long), sizeof(void *));
    system("pause");
    return 0;
}