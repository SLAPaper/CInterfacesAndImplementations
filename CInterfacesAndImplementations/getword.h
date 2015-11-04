#pragma once

#include <stdio.h>
#include <stdbool.h>

bool getword(FILE *fp, char *buf, int size, int first(int c), int rest(int c));