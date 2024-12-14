#ifndef cluminique_ast_h
#define cluminique_ast_h

#include <stdio.h>
#include <stdlib.h>

#include "../types/types.h"

#define NOREG -1

typedef enum AstNodeOp {
  AST_ADD = 1, 
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,
  
  AST_EQ,
  AST_NE,
  AST_LT,
  AST_GT,
  AST_LE,
  AST_GE,

  AST_INT_LIT,

  AST_IDENTIFIER,
  AST_LVIDENT,
  AST_ASSIGN,
  AST_PRINT,
  AST_GLUE,
  AST_IF,
  AST_WHILE,
  AST_FUNCTION,
  AST_WIDEN,
  AST_RETURN,
  AST_CALL,
  AST_DEREFERENCE,
  AST_ADDRESS
} AstNodeOp;

typedef struct ASTNode {
  AstNodeOp op;
  PrimitiveTypes type;
  struct ASTNode* left;
  struct ASTNode* mid;
  struct ASTNode* right;
  union {
    int intvalue;
    int id;
  } value;
} ASTNode;

ASTNode* createNode(AstNodeOp op, PrimitiveTypes type, ASTNode* left, ASTNode* mid, ASTNode* right, int value);
ASTNode* createLeafNode(AstNodeOp op, PrimitiveTypes type, int value);
ASTNode* createUnaryNode(AstNodeOp op, PrimitiveTypes type, ASTNode* child, int value);
AstNodeOp getArithmeticOperation(TokenType token);

int label();

bool isComparasionOperation(AstNodeOp op);

int generateAST(ASTNode* node, int reg, AstNodeOp parentASTOp);

#endif
