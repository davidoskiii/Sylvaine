#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "../common.h"

static int charPos(char* str, int chr) {
  char* pchar = strchr(str, chr);
  return (pchar ? pchar - str : -1);
}

static int next() {
  int c;

  if (compiler->putback) {
    c = compiler->putback;
    compiler->putback = 0;
    return c;
  }

  c = fgetc(compiler->fileI);
  if ('\n' == c) compiler->line++;
  return c;
}

static void putback(int chr) {
  compiler->putback = chr;
}

static int skip() {
  int currentChar = next();
  while (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' || 
          currentChar == '\r' || currentChar == '\f') {
    currentChar = next();
  }
  return currentChar;
}


static int scanint(int chr) {
  int index, result = 0;

  while ((index = charPos("0123456789", chr)) >= 0) {
    result = result * 10 + index;
    chr = next();
  }

  putback(chr);
  return result;
}


bool scan(Token* token) {
  int c = skip();

  switch (c) {
    case EOF:
      token->type = TOKEN_EOF;
      return false;
    case '+':
      token->type = TOKEN_PLUS;
      break;
    case '-':
      token->type = TOKEN_MINUS;
      break;
    case '*':
      token->type = TOKEN_STAR;
      break;
    case '/':
      token->type = TOKEN_SLASH;
      break;
    default: {
      if (isdigit(c)) {
        token->intvalue = scanint(c);
        token->type = TOKEN_INT_LIT;
        break;
      }

      printf("Unrecognised character %c on line %d\n", c, compiler->line);
      exit(1);
    }
  }

  return true;
}
