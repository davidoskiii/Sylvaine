

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../ast/ast.h"

static int OpPrec[] = { 0, 10, 10, 20, 20,    0 };
//                     EOF  +   -   *   /  INTLIT

static int getOpPrecedence(int tokenType) {
  int precedence = OpPrec[tokenType];
  if (precedence == 0) {
    fprintf(stderr, "Syntax error on line %d: invalid token %d\n", compiler->line, tokenType);
    exit(1);
  }
  return precedence;
}

static ASTNode* parsePrimary() {
  ASTNode* node;

  switch (compiler->current.type) {
    case TOKEN_INT_LIT:
      node = createLeafNode(AST_INT_LIT, compiler->current.intvalue);
      scan(&compiler->current);
      return node;
    default:
      fprintf(stderr, "syntax error on line %d\n", compiler->line);
      exit(1);
  }
}

ASTNode* parseBinaryExpression(int previousPrecedence) {
  ASTNode *leftNode, *rightNode;
  int tokenType;

  leftNode = parsePrimary();

  tokenType = compiler->current.type;
  if (tokenType == TOKEN_EOF)
    return leftNode;

  while (getOpPrecedence(tokenType) > previousPrecedence) {
    scan(&compiler->current);

    rightNode = parseBinaryExpression(OpPrec[tokenType]);

    leftNode = createNode(getArithmeticOperation(tokenType), leftNode, rightNode, 0);

    tokenType = compiler->current.type;
    if (tokenType == TOKEN_EOF)
      return leftNode;
  }

  // Return the tree when the precedence is the same or lower
  return leftNode;
}
