#include <stdio.h>
#include <string.h>

#include "misc.h"
#include "../common.h"
#include "../ast/ast.h"

void fatal(char* s) {
  fprintf(stderr, "%s on line %d\n", s, compiler->line); exit(1);
}

void fatals(char* s1, char* s2) {
  fprintf(stderr, "%s:%s on line %d\n", s1, s2, compiler->line); exit(1);
}

void fatald(char* s, int d) {
  fprintf(stderr, "%s:%d on line %d\n", s, d, compiler->line); exit(1);
}

void fatalc(char* s, int c) {
  fprintf(stderr, "%s:%c on line %d\n", s, c, compiler->line); exit(1);
}
