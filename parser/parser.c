

#include "parser.h"
#include "../common.h"
#include "../lexer/lexer.h"
#include "../ast/ast.h"

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


ASTNode* parseBinaryExpression() {
  ASTNode *node, *leftNode, *rightNode;
  AstNodeOp nodeType;

  leftNode = parsePrimary();

  if (compiler->current.type == TOKEN_EOF) return leftNode;

  nodeType = getArithmeticOperation(compiler->current.type);

  scan(&compiler->current);

  rightNode = parseBinaryExpression();

  node = createNode(nodeType, leftNode, rightNode, 0);
  return node;
}

