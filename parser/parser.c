#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../types/types.h"
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

void retreat() {
  rejectToken(&compiler->current);
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

PrimitiveType parseType(TokenType token) {
  PrimitiveType type;
  switch (token) {
    case TOKEN_CHAR: type = PRIMITIVE_CHAR; break;
    case TOKEN_INT:  type = PRIMITIVE_INT; break;
    case TOKEN_LONG:  type = PRIMITIVE_LONG; break;
    case TOKEN_VOID: type = PRIMITIVE_VOID; break;
    default:
      fatald("Illegal type, token", token);
  }

  while (true) {
    advance();
    if (compiler->current.type != TOKEN_STAR) {
      break;
    }
    type = toPointerType(type);
  }

  return type;
}

static int getOpPrecedence(TokenType tokenType) {
  int precedence = OpPrec[tokenType];
  if (precedence == 0) {
    fprintf(stderr, "Syntax error on line %d: invalid token %d\n", compiler->line, tokenType);
    exit(1);
  }
  return precedence;
}

ASTNode* parsePrefixExpression() {
  ASTNode* subtree;

  switch (compiler->current.type) {
    case TOKEN_AMPER:
      advance();
      subtree = parsePrefixExpression();

      if (subtree->op != AST_IDENTIFIER) {
        fatal("The '&' operator must be followed by an identifier.");
      }

      subtree->op = AST_ADDRESS;
      subtree->type = toPointerType(subtree->type);
      break;

    case TOKEN_STAR:
      advance();
      subtree = parsePrefixExpression();

      if (subtree->op != AST_IDENTIFIER && subtree->op != AST_DEREFERENCE) {
        fatal("The '*' operator must be followed by an identifier or another '*'.");
      }

      subtree = createUnaryNode(AST_DEREFERENCE, toValueType(subtree->type), subtree, 0);
      break;

    default:
      subtree = parsePrimaryExpression();
      break;
  }

  return subtree;
}

ASTNode* parsePrimaryExpression() {
  ASTNode* node;
  int id;

  switch (compiler->current.type) {
    case TOKEN_INT_LIT:
      if ((compiler->current.intvalue) >= 0 && (compiler->current.intvalue < 256)) node = createLeafNode(AST_INT_LIT, PRIMITIVE_CHAR, compiler->current.intvalue);
      else node = createLeafNode(AST_INT_LIT, PRIMITIVE_INT, compiler->current.intvalue);
      break;
    case TOKEN_IDENTIFIER:
      advance();
      
      if (compiler->current.type == TOKEN_LPAREN) {
        return parseFunctionCall();
      }

      retreat();
      
      id = findGlobalSymbol(compiler->buffer);
      if (id == -1) fatals("Unknown variable", compiler->buffer);

      node = createLeafNode(AST_IDENTIFIER, globalSymbolTable[id].type, id);
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

  tree = adjustType(tree, PRIMITIVE_INT, false);

  if (tree == NULL) {
    fatal("Incompatible types in print statement");
  }

  tree = createUnaryNode(AST_PRINT, PRIMITIVE_NONE, tree, 0);

  return tree;
}

ASTNode* parseGlobalDeclarations() {
  switch (compiler->current.type) {
    case TOKEN_FN: return parseFunctionDeclaration();
    case TOKEN_LET: {
      ASTNode* tree = parseVarDeclaration();
      if (tree != NULL) match(TOKEN_SEMICOLON, ";");
      return tree;
    }
    default:
      fatald("Syntax error, token", compiler->current.type);
  }
}

ASTNode* parseVarDeclaration() {
  ASTNode *left, *right, *tree;
  int id;
  PrimitiveType type;

  match(TOKEN_LET, "let");
  
  parseIdentifier();

  char* buffer = malloc(strlen(compiler->buffer) + 1);
  
  if (buffer == NULL) {
    fatal("Failed to allocate memory in parseVarDeclaration()");
  }
  
  strcpy(buffer, compiler->buffer);

  match(TOKEN_COLON, ":");
  type = parseType(compiler->current.type);

  id = addGlobalSymbol(buffer, type, STRUCTURAL_VARIABLE, 0);
  generateGlobalSymbol(id);

  if (check(TOKEN_EQUAL)) {
    if (compiler->depth == 0) fatal("Can't inizialize a global variable");
    match(TOKEN_EQUAL, "=");

    if ((id = findGlobalSymbol(buffer)) == -1) {
      fatals("Undeclared variable", buffer);
    }
    right = createLeafNode(AST_LVIDENT, type, id);

    left = parseBinaryExpression(0);

    left = adjustType(left, right->type, false);
    if (left == NULL)
      fatal("Incompatible types in parseVarDeclaration()");

    tree = createNode(AST_ASSIGN, PRIMITIVE_INT, left, NULL, right, 0);

    free(buffer);

    return tree;
  }

  match(TOKEN_SEMICOLON, ";");

  free(buffer);

  return NULL;
}

ASTNode* parseFunctionDeclaration() {
  ASTNode *tree, *finalStatement;
  int nameSlot, endLabel;
  PrimitiveType type;

  match(TOKEN_FN, "fn");

  parseIdentifier();

  char* buffer = malloc(strlen(compiler->buffer) + 1);
  
  if (buffer == NULL) {
    fatal("Failed to allocate memory in parseVarDeclaration()");
  }
  
  strcpy(buffer, compiler->buffer);

  match(TOKEN_LPAREN, "(");
  match(TOKEN_RPAREN, ")");

  match(TOKEN_ARROW, "->");

  type = parseType(compiler->current.type);

  endLabel = label();
  nameSlot = addGlobalSymbol(buffer, type, STRUCTURAL_FUNCTION, endLabel);
  compiler->functionId = nameSlot;

  tree = parseCompoundStatement();

  if (type != PRIMITIVE_VOID) {
    finalStatement = (tree->op == AST_GLUE) ? tree->right : tree;
    if (finalStatement == NULL || finalStatement->op != AST_RETURN)
      fatal("No return for function with non-void type");
  }

  free(buffer);

  return createUnaryNode(AST_FUNCTION, type, tree, nameSlot);
}

ASTNode* parseFunctionCall() {
  ASTNode* tree;
  int id;

  if ((id = findGlobalSymbol(compiler->buffer)) == -1) {
    fatals("Undeclared function", compiler->buffer);
  }
  match(TOKEN_LPAREN, "(");

  tree = parseBinaryExpression(0);

  tree = createUnaryNode(AST_CALL, globalSymbolTable[id].type, tree, id);

  match(TOKEN_RPAREN, ")");
  return tree;
}

static ASTNode* parseAssignmentStatement() {
  ASTNode *left, *right, *tree;
  int id;

  parseIdentifier();

  if (compiler->current.type == TOKEN_LPAREN)
    return parseFunctionCall();

  if ((id = findGlobalSymbol(compiler->buffer)) == -1) {
    fatals("Undeclared variable", compiler->buffer);
  }
  right = createLeafNode(AST_LVIDENT, globalSymbolTable[id].type, id);

  match(TOKEN_EQUAL, "=");

  left = parseBinaryExpression(0);

  left = adjustType(left, right->type, false);
  if (left == NULL)
    fatal("Incompatible types in parseVarDeclaration()");

  tree = createNode(AST_ASSIGN, PRIMITIVE_INT, left, NULL, right, 0);

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

  match(TOKEN_LPAREN, "(");

  condAST = parseConditionExpression();

  match(TOKEN_RPAREN, ")");

  trueAST = parseCompoundStatement();

  if (compiler->current.type == TOKEN_ELSE) {
    advance();
    falseAST = parseCompoundStatement();
  }

  return createNode(AST_IF, PRIMITIVE_NONE, condAST, trueAST, falseAST, 0);
}

ASTNode* parseWhileStatement() {
  ASTNode* condAST;
  ASTNode* bodyAST;

  match(TOKEN_WHILE, "while");

  match(TOKEN_LPAREN, "(");

  condAST = parseConditionExpression();

  match(TOKEN_RPAREN, ")");

  bodyAST = parseCompoundStatement();

  return createNode(AST_WHILE, PRIMITIVE_NONE, condAST, NULL, bodyAST, 0);
}

static ASTNode* parseForStatement() {
  ASTNode *condAST, *bodyAST;
  ASTNode *preopAST, *postopAST;
  ASTNode *tree;

  match(TOKEN_FOR, "for");
  match(TOKEN_LPAREN, "(");

  preopAST = parseStatement();
  match(TOKEN_SEMICOLON, ";");

  condAST = parseConditionExpression();
  match(TOKEN_SEMICOLON, ";");

  postopAST = parseStatement();
  match(TOKEN_RPAREN, ")");

  bodyAST = parseCompoundStatement();

  tree = createNode(AST_GLUE, PRIMITIVE_NONE, bodyAST, NULL, postopAST, 0);
  tree = createNode(AST_WHILE, PRIMITIVE_NONE, condAST, NULL, tree, 0);
  return createNode(AST_GLUE, PRIMITIVE_NONE, preopAST, NULL, tree, 0);
}


static ASTNode* parseReturnStatement() {
  ASTNode* tree;

  if (globalSymbolTable[compiler->functionId].type == PRIMITIVE_VOID)
    fatal("Can't return from a void function");

  match(TOKEN_RETURN, "return");

  tree = parseBinaryExpression(0);

  tree = adjustType(tree, globalSymbolTable[compiler->functionId].type, false);
  if (tree == NULL)
    fatal("Incompatible types in parseVarDeclaration()");

  tree = createUnaryNode(AST_RETURN, PRIMITIVE_NONE, tree, 0);

  return tree;
}

ASTNode* parseStatement() {
  switch (compiler->current.type) {
    case TOKEN_PRINT:
      return parsePrintStatement();
    case TOKEN_LET:
      return parseVarDeclaration();
    case TOKEN_IDENTIFIER:
      return parseAssignmentStatement();
    case TOKEN_IF:
      return parseIfStatement();
    case TOKEN_WHILE:
      return parseWhileStatement();
    case TOKEN_FOR:
      return parseForStatement();
    case TOKEN_RETURN:
      return parseReturnStatement();
    default:
      fatald("Syntax error, token", compiler->current.type);
  }
}

ASTNode* parseCompoundStatement() {
  ASTNode* left = NULL;
  ASTNode* tree;

  match(TOKEN_LBRACE, "{");
  compiler->depth++;

  while (true) {
    tree = parseStatement();
    if (tree != NULL && (
      tree->op == AST_PRINT ||
      tree->op == AST_ASSIGN ||
      tree->op == AST_RETURN ||
      tree->op == AST_CALL
    )) match(TOKEN_SEMICOLON, ";");

    // Add the new tree to the left tree
    if (tree != NULL) {
      if (left == NULL) {
        left = tree;
      } else {
        left = createNode(AST_GLUE, PRIMITIVE_NONE, left, NULL, tree, 0);
      }
    }

    if (compiler->current.type == TOKEN_RBRACE) {
      match(TOKEN_RBRACE, "}");
      compiler->depth--;
      return left;
    }
  }
}

ASTNode* parseBinaryExpression(int previousPrecedence) {
  ASTNode *leftNode, *rightNode;
  ASTNode *lTemp, *rTemp;
  AstNodeOp ASTop;
  TokenType tokenType;

  leftNode = parsePrefixExpression();

  tokenType = compiler->current.type;
  if (tokenType == TOKEN_SEMICOLON || tokenType == TOKEN_RPAREN)
    return leftNode;

  while (getOpPrecedence(tokenType) > previousPrecedence) {
    advance();

    rightNode = parseBinaryExpression(OpPrec[tokenType]);

    ASTop = getArithmeticOperation(tokenType);
    lTemp = adjustType(leftNode, rightNode->type, ASTop);
    rTemp = adjustType(rightNode, leftNode->type, ASTop);
    if (lTemp == NULL && rTemp == NULL)
      fatal("Incompatible types in binary expression");
    if (lTemp != NULL)
      leftNode = lTemp;
    if (rTemp != NULL)
      rightNode = rTemp;

    leftNode = createNode(getArithmeticOperation(tokenType), leftNode->type, leftNode, NULL, rightNode, 0);

    tokenType = compiler->current.type;
    if (tokenType == TOKEN_SEMICOLON || tokenType == TOKEN_RPAREN)
      return leftNode;
  }

  return leftNode;
}

