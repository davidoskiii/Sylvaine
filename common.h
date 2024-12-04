#ifndef cluminique_common_h
#define cluminique_common_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer/lexer.h"

#define ALLOCATE(type) (type*)malloc(sizeof(type))

typedef struct Compiler {
  int line;
  int putback;
  Token current;
  FILE* fileI;
} Compiler;

extern Compiler* compiler;

#endif
