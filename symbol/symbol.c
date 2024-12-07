#include <stdio.h>
#include <string.h>

#include "symbol.h"
#include "../common.h"
#include "../misc/misc.h"

SymbolTable globalSymbolTable[NSYMBOLS];
static int globalCount = 0;

int findGlobalSymbol(char* symbol) {
  int i;

  for (i = 0; i < globalCount; i++) {
    if (*symbol == *globalSymbolTable[i].name && !strcmp(symbol, globalSymbolTable[i].name))
      return i;
  }
  return -1;
}

static int createNewGlobal() {
  int position;

  if ((position = globalCount++) >= NSYMBOLS)
    fatal("Too many global symbols");
  return position;
}

int addGlobalSymbol(char* name, PrimitiveTypes type, StructuralTypes stype) {
  int slot;

  if ((slot = findGlobalSymbol(name)) != -1)
    return slot;

  slot = createNewGlobal();
  globalSymbolTable[slot].name = strdup(name);
  globalSymbolTable[slot].type = type;
  globalSymbolTable[slot].stype = stype;
  return slot;
}
