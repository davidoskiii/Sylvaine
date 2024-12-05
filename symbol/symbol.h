#ifndef cluminique_symbol_h
#define cluminique_symbol_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../ast/ast.h"

typedef struct SymbolTable {
  char* name;
} SymbolTable;

int findGlobalSymbol(char* symbol);
int addGlobalSymbol(char* name);

extern SymbolTable globalSymbolTable[NSYMBOLS];

#endif
