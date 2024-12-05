#ifndef cluminique_parser_h
#define cluminique_parser_h

#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "../ast/ast.h"

ASTNode* parseBinaryExpression(int previousPrecedence);

#endif
