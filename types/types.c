#include "../ast/ast.h"
#include "../codegen/codegen.h"
#include "../common.h"
#include "types.h"

int typeCompatible(PrimitiveTypes* left, PrimitiveTypes* right, int onlyRight) {
  int leftSize = getPrimitiveSize(*left);
  int rightSize = getPrimitiveSize(*right);

  // Types with zero size are incompatible
  if (leftSize == 0 || rightSize == 0) 
    return 0;

  // Same types are compatible
  if (*left == *right) {
    *left = PRIMITIVE_NONE;
    *right = PRIMITIVE_NONE;
    return 1;
  }

  // Widen smaller types to match larger types
  if (leftSize < rightSize) {
    *left = AST_WIDEN;
    *right = PRIMITIVE_NONE;
    return 1;
  }
  if (rightSize < leftSize) {
    if (onlyRight) 
      return 0;
    *left = PRIMITIVE_NONE;
    *right = AST_WIDEN;
    return 1;
  }

  // Types of the same size are compatible
  *left = PRIMITIVE_NONE;
  *right = PRIMITIVE_NONE;
  return 1;
}
