#ifndef cluminique_common_h
#define cluminique_common_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer/lexer.h"

#define ALLOCATE(type) (type*)malloc(sizeof(type))
#define TEXTLEN 512
#define NSYMBOLS 512

typedef struct Compiler {
  int line;
  int putback;
  int depth;
  char buffer[TEXTLEN + 1];

  Token current;
  int functionId;

  FILE* fileI;
  FILE* fileO;
} Compiler;

extern Compiler* compiler;

#endif
