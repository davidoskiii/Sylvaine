#include "ast.h"

#include "../common.h"
#include "../lexer/lexer.h"
#include "../symbol/symbol.h"
#include "../codegen/codegen.h"

ASTNode* createNode(AstNodeOp op, ASTNode* left, ASTNode* mid, ASTNode* right, int value) {
  ASTNode* node = ALLOCATE(ASTNode);

  if (node == NULL) {
    fprintf(stderr, "Memory allocation failed in createNode()\n");
    exit(1);
  }

  node->op = op;
  node->left = left;
  node->mid = mid;
  node->right = right;
  node->value.intvalue = value;
  return node;
}


// Create an AST leaf node
ASTNode* createLeafNode(AstNodeOp op, int value) {
  return createNode(op, NULL, NULL, NULL, value);
}

// Create a unary AST node with one child
ASTNode* createUnaryNode(AstNodeOp op, ASTNode* child, int value) {
  return createNode(op, child, NULL, NULL, value);
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
    case TOKEN_EQ:
      return AST_EQ;
    case TOKEN_NE:
      return AST_NE;
    case TOKEN_LT:
      return AST_LT;
    case TOKEN_GT:
      return AST_GT;
    case TOKEN_LE:
      return AST_LE;
    case TOKEN_GE:
      return AST_GE;
    default:
      fprintf(stderr, "Unknown token in getArithmeticOperation() on line %d\n", compiler->line);
      exit(1);
  }
}

bool isComparasionOperation(AstNodeOp op) {
  switch (op) {
    case AST_EQ:
    case AST_NE:
    case AST_LT:
    case AST_GT:
    case AST_LE:
    case AST_GE:
      return true;
    default:
      return false;
  }
}

static int label() {
  static int id = 1;
  return id++;
}

static int generateIfAST(ASTNode* node) {
  int falseLabel = label();
  int endLabel = node->right ? label() : falseLabel;

  generateAST(node->left, falseLabel, node->op);
  freeRegisters();

  generateAST(node->mid, NOREG, node->op);
  freeRegisters();

  if (node->right) generateJump(endLabel);

  generateLabel(falseLabel);

  if (node->right) {
    generateAST(node->right, NOREG, node->op);
    freeRegisters();
    generateLabel(endLabel);
  }

  return NOREG;
}

int generateAST(ASTNode* node, int reg, AstNodeOp parentASTOp) {
  int leftRegister, rightRegister;

  switch (node->op) {
    case AST_IF:
      return generateIfAST(node);
    case AST_GLUE:
      generateAST(node->left, NOREG, node->op);
      freeRegisters();
      generateAST(node->right, NOREG, node->op);
      freeRegisters();
      return NOREG;
    default: break;
  }

  if (node->left)
    leftRegister = generateAST(node->left, -1, node->op);
  if (node->right)
    rightRegister = generateAST(node->right, leftRegister, node->op);

  switch (node->op) {
    case AST_ADD:
      return generateAddition(leftRegister, rightRegister);
    case AST_SUBTRACT:
      return generateSubtraction(leftRegister, rightRegister);
    case AST_MULTIPLY:
      return generateMultiplication(leftRegister, rightRegister);
    case AST_DIVIDE:
      return generateDivision(leftRegister, rightRegister);
    case AST_EQ:
    case AST_NE:
    case AST_LT:
    case AST_GT:
    case AST_LE:
    case AST_GE:
      if (parentASTOp == AST_IF) return compareAndJump(node->op, leftRegister, rightRegister, reg);
      else return compareAndSet(node->op, leftRegister, rightRegister);
    case AST_INT_LIT:
      return generateLoadInteger(node->value.intvalue);
    case AST_IDENTIFIER:
      return loadGlobalSymbol(globalSymbolTable[node->value.id].name);
    case AST_LVIDENT:
      return storeGlobalSymbol(reg, globalSymbolTable[node->value.id].name);
    case AST_ASSIGN:
      return rightRegister;
    case AST_PRINT:
      generatePrintInteger(leftRegister);
      freeRegisters();
      return NOREG;
    default:
      fprintf(stderr, "Unknown AST operation %d\n", node->op);
      exit(1);
  }
}
