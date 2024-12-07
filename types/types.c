#include "../ast/ast.h"
#include "../common.h"
#include "types.h"

int typeCompatible(PrimitiveTypes* left, PrimitiveTypes* right, int onlyRight) {
  if (*left == PRIMITIVE_VOID || *right == PRIMITIVE_VOID) 
    return 0;

  if (*left == *right) {
    *left = PRIMITIVE_NONE;
    *right = PRIMITIVE_NONE;
    return 1;
  }

  if (*left == PRIMITIVE_CHAR && *right == PRIMITIVE_INT) {
    *left = AST_WIDEN;
    *right = PRIMITIVE_NONE;
    return 1;
  }

  if (*left == PRIMITIVE_INT && *right == PRIMITIVE_CHAR) {
    if (onlyRight) 
      return 0;
    *left = PRIMITIVE_NONE;
    *right = AST_WIDEN;
    return 1;
  }

  *left = PRIMITIVE_NONE;
  *right = PRIMITIVE_NONE;
  return 1;
}
