#ifndef cluminique_lexer_h
#define cluminique_lexer_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum TokenType {
  TOKEN_EOF,
  TOKEN_PLUS, 
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_INT_LIT,
  TOKEN_SEMICOLON,
  TOKEN_PRINT
} TokenType;

typedef struct Token {
  TokenType type;
  int intvalue;
} Token;

bool scan(Token* token);

#endif
