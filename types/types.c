#include "../codegen/codegen.h"
#include "../misc/misc.h"
#include "../common.h"
#include "types.h"

bool isIntegerType(PrimitiveType type) {
  return (type == PRIMITIVE_CHAR || type == PRIMITIVE_INT || type == PRIMITIVE_LONG);
}

bool isPointerType(PrimitiveType type) {
  return (type == PRIMITIVE_VOID_POINTER || type == PRIMITIVE_CHAR_POINTER ||
          type == PRIMITIVE_INT_POINTER || type == PRIMITIVE_LONG_POINTER);
}

PrimitiveType toPointerType(PrimitiveType type) {
  switch (type) {
    case PRIMITIVE_VOID: return PRIMITIVE_VOID_POINTER;
    case PRIMITIVE_CHAR: return PRIMITIVE_CHAR_POINTER;
    case PRIMITIVE_INT:  return PRIMITIVE_INT_POINTER;
    case PRIMITIVE_LONG: return PRIMITIVE_LONG_POINTER;
    default:
      fatald("Unrecognized type in toPointerType()", type);
  }
}

PrimitiveType toValueType(PrimitiveType type) {
  switch (type) {
    case PRIMITIVE_VOID_POINTER: return PRIMITIVE_VOID;
    case PRIMITIVE_CHAR_POINTER: return PRIMITIVE_CHAR;
    case PRIMITIVE_INT_POINTER:  return PRIMITIVE_INT;
    case PRIMITIVE_LONG_POINTER: return PRIMITIVE_LONG;
    default:
      fatald("Unrecognized type in valueAt()", type);
  }
}

PrimitiveType dereferencePointerType(PrimitiveType type) {
  switch (type) {
    case PRIMITIVE_VOID_POINTER: return PRIMITIVE_VOID;
    case PRIMITIVE_CHAR_POINTER: return PRIMITIVE_CHAR;
    case PRIMITIVE_INT_POINTER:  return PRIMITIVE_INT;
    case PRIMITIVE_LONG_POINTER: return PRIMITIVE_LONG;
    default:
      fatald("Unrecognized type in dereferencePointerType()", type);
  }
}

ASTNode* adjustType(ASTNode* tree, PrimitiveType targetType, int binaryOp) {
  PrimitiveType sourceType = tree->type;
  int sourceSize, targetSize;

  if (isIntegerType(sourceType) && isIntegerType(targetType)) {
    if (sourceType == targetType) {
      return tree;
    }

    sourceSize = getPrimitiveSize(sourceType);
    targetSize = getPrimitiveSize(targetType);

    if (sourceSize > targetSize) {
      return NULL;
    }

    if (targetSize > sourceSize) {
      return createUnaryNode(AST_WIDEN, targetType, tree, 0);
    }
  }

  if (isPointerType(sourceType)) {
    if (binaryOp == 0 && sourceType == targetType) {
      return tree;
    }
  }

  if (binaryOp == AST_ADD || binaryOp == AST_SUBTRACT) {
    if (isIntegerType(sourceType) && isPointerType(targetType)) {
      targetSize = getPrimitiveSize(dereferencePointerType(targetType));
      if (targetSize > 1) {
        return createUnaryNode(AST_SCALE, targetType, tree, targetSize);
      }
    }
  }

  return NULL;
}
