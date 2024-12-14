#ifndef cluminique_lexer_h
#define cluminique_lexer_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum TokenType {
  TOKEN_EOF, // 0
  TOKEN_PLUS, // 1
  TOKEN_MINUS, // 2
  TOKEN_STAR, // 3
  TOKEN_SLASH, // 4

  TOKEN_EQ, // 5
  TOKEN_NE, // 6
  TOKEN_LT, // 7
  TOKEN_GT, // 8
  TOKEN_LE, // 9
  TOKEN_GE, // 10

  TOKEN_INT_LIT, // 11
  TOKEN_IDENTIFIER, // 12

  TOKEN_LBRACE, // 13
  TOKEN_RBRACE, // 14
  TOKEN_LPAREN, // 15
  TOKEN_RPAREN, // 16

  TOKEN_SEMICOLON, // 17
  TOKEN_EQUAL, // 18
  TOKEN_COLON, // 19
  TOKEN_ARROW, // 20
  TOKEN_LOGAND, // 21
  TOKEN_AMPER, // 22

  TOKEN_IF, // 23
  TOKEN_ELSE, // 24
  TOKEN_WHILE, // 25
  TOKEN_FOR, // 26
  TOKEN_RETURN, // 27
  TOKEN_PRINT, // 28
  TOKEN_FN, // 29
  TOKEN_LET, // 30

  TOKEN_INT, // 31
  TOKEN_CHAR, // 32
  TOKEN_LONG, // 33
  TOKEN_VOID // 34
} TokenType;

typedef struct Token {
  TokenType type;
  int intvalue;
} Token;

void rejectToken(Token* token);
bool scan(Token* token);

#endif
