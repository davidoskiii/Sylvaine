#include "../ast/ast.h"
#include "../codegen/codegen.h"
#include "../misc/misc.h"
#include "../common.h"
#include "types.h"

bool typeCompatible(PrimitiveTypes* left, PrimitiveTypes* right, bool onlyRight) {
  // Same types are compatible
  if (*left == *right) {
    *left = PRIMITIVE_NONE;
    *right = PRIMITIVE_NONE;
    return true;
  }

  int leftSize = getPrimitiveSize(*left);
  int rightSize = getPrimitiveSize(*right);

  // Types with zero size are incompatible
  if (leftSize == 0 || rightSize == 0) {
    return false;
  }

  // Widen smaller types to match larger types
  if (leftSize < rightSize) {
    *left = AST_WIDEN;
    *right = PRIMITIVE_NONE;
    return true;
  }

  if (rightSize < leftSize) {
    if (onlyRight) {
      return false;
    }
    *left = PRIMITIVE_NONE;
    *right = AST_WIDEN;
    return true;
  }

  // Types of the same size are compatible
  *left = PRIMITIVE_NONE;
  *right = PRIMITIVE_NONE;
  return true;
}

PrimitiveTypes pointerTo(PrimitiveTypes type) {
  switch (type) {
    case PRIMITIVE_VOID: return PRIMITIVE_VOID_POINTER;
    case PRIMITIVE_CHAR: return PRIMITIVE_CHAR_POINTER;
    case PRIMITIVE_INT:  return PRIMITIVE_INT_POINTER;
    case PRIMITIVE_LONG: return PRIMITIVE_LONG_POINTER;
    default:
      fatald("Unrecognized type in pointerTo()", type);
  }
}

PrimitiveTypes valueAt(PrimitiveTypes type) {
  switch (type) {
    case PRIMITIVE_VOID_POINTER: return PRIMITIVE_VOID;
    case PRIMITIVE_CHAR_POINTER: return PRIMITIVE_CHAR;
    case PRIMITIVE_INT_POINTER:  return PRIMITIVE_INT;
    case PRIMITIVE_LONG_POINTER: return PRIMITIVE_LONG;
    default:
      fatald("Unrecognized type in valueAt()", type);
  }
}
