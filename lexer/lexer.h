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
  TOKEN_ARROW,

  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_RETURN,
  TOKEN_PRINT,
  TOKEN_FN,
  TOKEN_LET,

  TOKEN_INT,
  TOKEN_CHAR,
  TOKEN_LONG,
  TOKEN_VOID
} TokenType;

typedef struct Token {
  TokenType type;
  int intvalue;
} Token;

void rejectToken(Token* token);
bool scan(Token* token);

#endif
