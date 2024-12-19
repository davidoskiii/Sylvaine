#ifndef cluminique_types_h
#define cluminique_types_h

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

typedef struct ASTNode ASTNode;

typedef enum PrimitiveType {
  PRIMITIVE_NONE,
  PRIMITIVE_VOID,
  PRIMITIVE_CHAR,
  PRIMITIVE_INT,
  PRIMITIVE_LONG,

  PRIMITIVE_VOID_POINTER,
  PRIMITIVE_CHAR_POINTER,
  PRIMITIVE_INT_POINTER,
  PRIMITIVE_LONG_POINTER
} PrimitiveType;

typedef enum StructuralType {
  STRUCTURAL_VARIABLE,
  STRUCTURAL_FUNCTION
} StructuralType;

bool isIntegerType(PrimitiveType type);
bool isPointerType(PrimitiveType type);
PrimitiveType toPointerType(PrimitiveType type);
PrimitiveType toValueType(PrimitiveType type);
PrimitiveType dereferencePointerType(PrimitiveType type);
ASTNode* adjustType(ASTNode* tree, PrimitiveType targetType, int binaryOp);

#endif
