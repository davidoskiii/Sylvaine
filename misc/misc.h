#ifndef cluminique_misc_h
#define cluminique_misc_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../ast/ast.h"

void fatal(char* s);

void fatals(char* s1, char* s2);

void fatald(char* s, int d);

void fatalc(char* s, int c);

#endif
