#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "../common.h"
#include "../misc/misc.h"

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

static int scanidentifier(int currentChar, char *buffer, int limit) {
  int length = 0;

  while (isalpha(currentChar) || isdigit(currentChar) || currentChar == '_') {
    if (length >= limit - 1) {
      printf("Identifier too long on line %d\n", compiler->line);
      exit(1);
    }
    buffer[length++] = currentChar;
    currentChar = next();
  }

  putback(currentChar);
  buffer[length] = '\0';
  return length;
}

static TokenType getKeywordToken(char* word) {
  switch (*word) {
    case 'c':
      if (!strcmp(word, "char")) return TOKEN_CHAR;
      break;
    case 'e':
      if (!strcmp(word, "else")) return TOKEN_ELSE;
      break;
    case 'f':
      if (!strcmp(word, "for")) return TOKEN_FOR;
      if (!strcmp(word, "fn")) return TOKEN_FN;
      break;
    case 'i':
      if (!strcmp(word, "if")) return (TOKEN_IF);
      if (!strcmp(word, "int")) return (TOKEN_INT);
      break;
    case 'l':
      if (!strcmp(word, "let")) return (TOKEN_LET);
      break;
    case 'p':
      if (!strcmp(word, "print")) return TOKEN_PRINT;
      break;
    case 'v':
      if (!strcmp(word, "void")) return TOKEN_VOID;
      break;
    case 'w':
      if (!strcmp(word, "while")) return TOKEN_WHILE;
      break;
  }
  return 0;
}


bool scan(Token* token) {
  int c = skip();
  TokenType tokenType;

  switch (c) {
    case EOF:
      token->type = TOKEN_EOF;
      return false;
    case '+':
      token->type = TOKEN_PLUS;
      break;
    case '-':
      if ((c = next()) == '>') {
        token->type = TOKEN_ARROW;
      } else {
        putback(c);
        token->type = TOKEN_MINUS;
      }
      break;
    case '*':
      token->type = TOKEN_STAR;
      break;
    case '/':
      token->type = TOKEN_SLASH;
      break;
    case '=':
      if ((c = next()) == '=') {
        token->type = TOKEN_EQ;
      } else {
        putback(c);
        token->type = TOKEN_EQUAL;
      }
      break;
    case '!':
      if ((c = next()) == '=') {
        token->type = TOKEN_NE;
      } else {
        fatalc("Unrecognised character", c);
      }
      break;
    case '<':
      if ((c = next()) == '=') {
        token->type = TOKEN_LE;
      } else {
        putback(c);
        token->type = TOKEN_LT;
      }
      break;
    case '>':
      if ((c = next()) == '=') {
        token->type = TOKEN_GE;
      } else {
        putback(c);
        token->type = TOKEN_GT;
      }
      break;
    case '{':
      token->type = TOKEN_LBRACE;
      break;
    case '}':
      token->type = TOKEN_RBRACE;
      break;
    case '(':
      token->type = TOKEN_LPAREN;
      break;
    case ')':
      token->type = TOKEN_RPAREN;
      break;
    case ':':
      token->type = TOKEN_COLON;
      break;
    case ';':
      token->type = TOKEN_SEMICOLON;
      break;
    default:

      if (isdigit(c)) {
        token->intvalue = scanint(c);
        token->type = TOKEN_INT_LIT;
        break;
      } else if (isalpha(c) || '_' == c) {
        scanidentifier(c, compiler->buffer, TEXTLEN);

        if ((tokenType = getKeywordToken(compiler->buffer))) {
          token->type= tokenType;
          break;
        }

        token->type = TOKEN_IDENTIFIER;
        break;
      }
      printf("Unrecognised character %c on line %d\n", c, compiler->line);
      exit(1);
  }

  return true;
}
