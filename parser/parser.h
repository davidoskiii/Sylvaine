#ifndef cluminique_parser_h
#define cluminique_parser_h

#include "../ast/ast.h"

ASTNode* parseBinaryExpression(int previousPrecedence);
void parseStatements();

void advance();
void match(int tokenType, char* expected);
bool check(int tokenType);

#endif
