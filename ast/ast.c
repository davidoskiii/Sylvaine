#include "ast.h"

#include "../common.h"
#include "../lexer/lexer.h"
#include "../codegen/codegen.h"

ASTNode* createNode(AstNodeOp op, ASTNode* left, ASTNode* right, int value) {
  ASTNode* node = ALLOCATE(ASTNode);

  if (node == NULL) {
    fprintf(stderr, "Memory allocation failed in createNode()\n");
    exit(1);
  }

  node->op = op;
  node->left = left;
  node->right = right;
  node->intvalue = value;
  return node;
}


// Create an AST leaf node
ASTNode* createLeafNode(AstNodeOp op, int value) {
  return createNode(op, NULL, NULL, value);
}

// Create a unary AST node with one child
ASTNode* createUnaryNode(AstNodeOp op, ASTNode* child, int value) {
  return createNode(op, child, NULL, value);
}

AstNodeOp getArithmeticOperation(TokenType token) {
  switch (token) {
    case TOKEN_PLUS:
      return AST_ADD;
    case TOKEN_MINUS:
      return AST_SUBTRACT;
    case TOKEN_STAR:
      return AST_MULTIPLY;
    case TOKEN_SLASH:
      return AST_DIVIDE;
    default:
      fprintf(stderr, "Unknown token in getArithmeticOperation() on line %d\n", compiler->line);
      exit(1);
  }
}

int generateAST(ASTNode* node) {
  int leftRegister, rightRegister;

  if (node->left)
    leftRegister = generateAST(node->left);
  if (node->right)
    rightRegister = generateAST(node->right);

  switch (node->op) {
    case AST_ADD:
      return generateAddition(leftRegister, rightRegister);
    case AST_SUBTRACT:
      return generateSubtraction(leftRegister, rightRegister);
    case AST_MULTIPLY:
      return generateMultiplication(leftRegister, rightRegister);
    case AST_DIVIDE:
      return generateDivision(leftRegister, rightRegister);
    case AST_INT_LIT:
      return generateLoadInteger(node->intvalue);
    default:
      fprintf(stderr, "Unknown AST operation %d\n", node->op);
      exit(1);
  }
}
