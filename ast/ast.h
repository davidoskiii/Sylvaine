#ifndef cluminique_ast_h
#define cluminique_ast_h

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"

typedef enum AstNodeOp {
  AST_ADD, 
  AST_SUBTRACT,
  AST_MULTIPLY,
  AST_DIVIDE,
  AST_INT_LIT,
  AST_IDENTIFIER,
  AST_LVIDENT,
  AST_ASSIGN
} AstNodeOp;

typedef struct ASTNode {
  AstNodeOp op;
  struct ASTNode* left;
  struct ASTNode* right;
  union {
    int intvalue;
    int id;
  } value;
} ASTNode;

ASTNode* createNode(AstNodeOp op, ASTNode* left, ASTNode* right, int value);
ASTNode* createLeafNode(AstNodeOp op, int value);
ASTNode* createUnaryNode(AstNodeOp op, ASTNode* child, int value);
AstNodeOp getArithmeticOperation(TokenType token);

int generateAST(ASTNode* node, int reg);

#endif
