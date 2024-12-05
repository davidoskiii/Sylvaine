#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../codegen/codegen.h"
#include "../ast/ast.h"

static int OpPrec[] = { 0, 10, 10, 20, 20,    0 };
//                     EOF  +   -   *   /  INTLIT

void match(int tokenType, char *expected) {
  if (compiler->current.type == tokenType) {
    scan(&compiler->current);
  } else {
    printf("%s expected on line %d\n", expected, compiler->line);
    exit(1);
  }
}

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

void parseStatements() {
  ASTNode* tree;
  int reg;

  while (true) {
    match(TOKEN_PRINT, "print");
    tree = parseBinaryExpression(0);
    reg = generateAST(tree);
    generatePrintInteger(reg);
    freeRegisters();

    match(TOKEN_SEMICOLON, ";");
    if (compiler->current.type == TOKEN_EOF)
      return;
  }
}

ASTNode* parseBinaryExpression(int previousPrecedence) {
  ASTNode *leftNode, *rightNode;
  TokenType tokenType;

  leftNode = parsePrimary();

  tokenType = compiler->current.type;
  if (tokenType == TOKEN_SEMICOLON)
    return leftNode;

  while (getOpPrecedence(tokenType) > previousPrecedence) {
    scan(&compiler->current);

    rightNode = parseBinaryExpression(OpPrec[tokenType]);

    leftNode = createNode(getArithmeticOperation(tokenType), leftNode, rightNode, 0);

    tokenType = compiler->current.type;
    if (tokenType == TOKEN_SEMICOLON)
      return leftNode;
  }

  // Return the tree when the precedence is the same or lower
  return leftNode;
}

