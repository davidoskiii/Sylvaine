#ifndef cluminique_parser_h
#define cluminique_parser_h

#include "../ast/ast.h"

void parseStatements();

ASTNode* parseBinaryExpression(int previousPrecedence);
ASTNode* parseGlobalDeclarations();
ASTNode* parseFunctionDeclaration();
ASTNode* parseFunctionCall();
ASTNode* parseCompoundStatement();
ASTNode* parseStatement();
ASTNode* parsePrimaryExpression();
ASTNode* parseVarDeclaration();

void advance();
void match(int tokenType, char* expected);
bool check(int tokenType);

#endif
