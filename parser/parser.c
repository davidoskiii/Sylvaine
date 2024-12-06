#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../misc/misc.h"
#include "../symbol/symbol.h"
#include "../codegen/codegen.h"
#include "../ast/ast.h"

static int OpPrec[] = {
  0, 10, 10,                    // T_EOF, T_PLUS, T_MINUS
  20, 20,                       // T_STAR, T_SLASH
  30, 30,                       // T_EQ, T_NE
  40, 40, 40, 40                // T_LT, T_GT, T_LE, T_GE
};

void advance() {
  scan(&compiler->current);
}

void match(int tokenType, char* expected) {
  if (compiler->current.type == tokenType) {
    advance();
  } else {
    printf("%s expected on line %d\n", expected, compiler->line);
    exit(1);
  }
}

bool check(int tokenType) {
  if (compiler->current.type == tokenType) {
    return true;
  } else {
    return false;
  }
}

void parseIdentifier() {
  match(TOKEN_IDENTIFIER, "identifier");
}

static int getOpPrecedence(TokenType tokenType) {
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

  advance();
  return node;
}

static ASTNode* parsePrintStatement() {
  ASTNode* tree;

  match(TOKEN_PRINT, "print");

  tree = parseBinaryExpression(0);
  tree = createUnaryNode(AST_PRINT, tree, 0);

  match(TOKEN_SEMICOLON, ";");
  return tree;
}

static ASTNode* parseVarDeclaration() {
  int id;

  match(TOKEN_LET, "let");
  
  parseIdentifier();

  addGlobalSymbol(compiler->buffer);
  generateGlobalSymbol(compiler->buffer);

  if ((id = findGlobalSymbol(compiler->buffer)) == -1) {
    fatals("Undeclared variable", compiler->buffer);
  }

  match(TOKEN_COLON, ":");
  match(TOKEN_INT, "int");

  if (check(TOKEN_EQUAL)) {
    ASTNode *left, *right, *tree;

    right = createLeafNode(AST_LVIDENT, id);

    match(TOKEN_EQUAL, "=");

    left = parseBinaryExpression(0);

    tree = createNode(AST_ASSIGN, left, NULL, right, 0);

    match(TOKEN_SEMICOLON, ";");

    return tree;
  }

  match(TOKEN_SEMICOLON, ";");

  return NULL;
}

static ASTNode* parseAssignmentStatement() {
  ASTNode *left, *right, *tree;
  int id;

  parseIdentifier();

  if ((id = findGlobalSymbol(compiler->buffer)) == -1) {
    fatals("Undeclared variable", compiler->buffer);
  }
  right = createLeafNode(AST_LVIDENT, id);

  match(TOKEN_EQUAL, "=");

  left = parseBinaryExpression(0);

  tree = createNode(AST_ASSIGN, left, NULL, right, 0);

  match(TOKEN_SEMICOLON, ";");

  return tree;
}


ASTNode* parseConditionExpression() {
  ASTNode* condition = parseBinaryExpression(0);
  
  // Ensure the parsed condition is a valid comparison
  if (!isComparasionOperation(condition->op)) {
    fatal("Invalid comparison operator in condition");
  }

  return condition;
}

ASTNode* parseIfStatement() {
  ASTNode *condAST, *trueAST, *falseAST = NULL;

  match(TOKEN_IF, "if");

  condAST = parseConditionExpression();

  trueAST = parseCompoundStatement();

  if (compiler->current.type == TOKEN_ELSE) {
    advance();
    falseAST = parseCompoundStatement();
  }

  return createNode(AST_IF, condAST, trueAST, falseAST, 0);
}

ASTNode* parseCompoundStatement() {
  ASTNode* left = NULL;
  ASTNode* tree;

  match(TOKEN_LBRACE, "{");

  while (true) {
    switch (compiler->current.type) {
      case TOKEN_PRINT:
        tree = parsePrintStatement();
        break;
      case TOKEN_LET:
        tree = parseVarDeclaration();
        break;
      case TOKEN_IDENTIFIER:
        tree = parseAssignmentStatement();
        break;
      case TOKEN_IF:
        tree = parseIfStatement();
        break;
      case TOKEN_RBRACE:
        match(TOKEN_RBRACE, "}");
        return left;
      default:
        fatald("Syntax error, token", compiler->current.type);
    }

    // Add the new tree to the left tree
    if (tree) {
      if (!left) {
        left = tree;
      } else {
        left = createNode(AST_GLUE, left, NULL, tree, 0);
      }
    }
  }
}

ASTNode* parseBinaryExpression(int previousPrecedence) {
  ASTNode *leftNode, *rightNode;
  TokenType tokenType;

  leftNode = parsePrimary();

  tokenType = compiler->current.type;
  if (tokenType == TOKEN_SEMICOLON || tokenType == TOKEN_LBRACE)
    return leftNode;

  while (getOpPrecedence(tokenType) > previousPrecedence) {
    advance();

    rightNode = parseBinaryExpression(OpPrec[tokenType]);

    leftNode = createNode(getArithmeticOperation(tokenType), leftNode, NULL, rightNode, 0);

    tokenType = compiler->current.type;
    if (tokenType == TOKEN_SEMICOLON || tokenType == TOKEN_LBRACE)
      return leftNode;
  }

  // Return the tree when the precedence is the same or lower
  return leftNode;
}

