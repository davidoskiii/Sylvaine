#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../misc/misc.h"
#include "../symbol/symbol.h"
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

void parseIdentifier() {
  match(TOKEN_IDENTIFIER, "identifier");
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
  int id;

  switch (compiler->current.type) {
    case TOKEN_INT_LIT:
      node = createLeafNode(AST_INT_LIT, compiler->current.intvalue);
      break;
    case TOKEN_IDENTIFIER:
      id = findGlobalSymbol(compiler->buffer);
      if (id == -1) fatals("Unknown variable", compiler->buffer);

      node = createLeafNode(AST_IDENTIFIER, id);
      break;
    default: fatald("Syntax error, token", compiler->current.type);
  }

  scan(&compiler->current);
  return node;
}

void parsePrintStatement() {
  ASTNode* tree;
  int reg;

  match(TOKEN_PRINT, "print");

  tree = parseBinaryExpression(0);
  reg = generateAST(tree, -1);
  generatePrintInteger(reg);
  freeRegisters();

  match(TOKEN_SEMICOLON, ";");
}

void parseVarDeclaration() {
  match(TOKEN_LET, "let");
  
  parseIdentifier();

  addGlobalSymbol(compiler->buffer);
  generateGlobalSymbol(compiler->buffer);

  match(TOKEN_COLON, ":");
  match(TOKEN_INT, "int");

  match(TOKEN_SEMICOLON, ";");
}

void parseAssignmentStatement() {
  ASTNode *left, *right, *tree;
  int id;

  parseIdentifier();

  if ((id = findGlobalSymbol(compiler->buffer)) == -1) {
    fatals("Undeclared variable", compiler->buffer);
  }
  right = createLeafNode(AST_LVIDENT, id);

  match(TOKEN_EQUAL, "=");

  left = parseBinaryExpression(0);

  tree = createNode(AST_ASSIGN, left, right, 0);

  generateAST(tree, -1);
  freeRegisters();

  match(TOKEN_SEMICOLON, ";");
}

void parseStatements() {
  while (true) {
    switch (compiler->current.type) {
    case TOKEN_PRINT:
      parsePrintStatement();
      break;
    case TOKEN_LET:
      parseVarDeclaration();
      break;
    case TOKEN_IDENTIFIER:
      parseAssignmentStatement();
      break;
    case TOKEN_EOF:
      return;
    default:
      fatald("Syntax error, token", compiler->current.type);
    }
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

