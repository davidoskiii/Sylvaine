#include <string.h>
#include <errno.h>

#include "common.h"
#include "lexer/lexer.h"
#include "ast/ast.h"
#include "parser/parser.h"

Compiler* compiler;

static void init() {
  compiler = ALLOCATE(Compiler);

  compiler->line = 1;
  compiler->putback = '\n';
  compiler->fileI = NULL;
}

static void usage(char* prog) {
  fprintf(stderr, "Usage: %s infile\n", prog);
  exit(1);
}

int main(int argc, char *argv[]) {
  ASTNode* node;

  if (argc != 2)
    usage(argv[0]);

  init();

  if ((compiler->fileI = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
    exit(1);
  }

  scan(&compiler->current);
  node = parseBinaryExpression();
  printf("%d\n", interpretAST(node));
  exit(0);
  return 0;
}
