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

  TOKEN_EQ,
  TOKEN_NE,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LE,
  TOKEN_GE,

  TOKEN_INT_LIT,
  TOKEN_IDENTIFIER,

  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LPAREN,
  TOKEN_RPAREN,

  TOKEN_SEMICOLON,
  TOKEN_EQUAL,
  TOKEN_COLON,

  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_PRINT,
  TOKEN_LET,
  TOKEN_INT
} TokenType;

typedef struct Token {
  TokenType type;
  int intvalue;
} Token;

bool scan(Token* token);

#endif
