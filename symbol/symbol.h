#ifndef cluminique_symbol_h
#define cluminique_symbol_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../ast/ast.h"
#include "../types/types.h"

typedef struct SymbolTable {
  char* name;
  PrimitiveTypes type;
  StructuralTypes stype;
  int endLabel;
} SymbolTable;

int findGlobalSymbol(char* symbol);
int addGlobalSymbol(char* name, PrimitiveTypes type, StructuralTypes stype, int endLabel);

extern SymbolTable globalSymbolTable[NSYMBOLS];

#endif
