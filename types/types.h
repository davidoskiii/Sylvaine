#ifndef cluminique_types_h
#define cluminique_types_h

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

typedef enum PrimitiveTypes {
  PRIMITIVE_NONE,
  PRIMITIVE_VOID,
  PRIMITIVE_CHAR,
  PRIMITIVE_INT,
  PRIMITIVE_LONG
} PrimitiveTypes;

typedef enum StructuralTypes {
  STRUCTURAL_VARIABLE,
  STRUCTURAL_FUNCTION
} StructuralTypes;

int typeCompatible(PrimitiveTypes* left, PrimitiveTypes* right, int onlyRight);

#endif
