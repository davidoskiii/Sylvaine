#ifndef cluminique_codegen_h
#define cluminique_codegen_h

#include "../ast/ast.h"


void generateCode(ASTNode* node);

void freeRegisters();
void generateAssemblyPreamble();
void generateAssemblyPostamble();

int generateLoadInteger(int value);
int generateAddition(int reg1, int reg2);
int generateSubtraction(int reg1, int reg2);
int generateMultiplication(int reg1, int reg2);
int generateDivision(int reg1, int reg2);

void generatePrintInteger(int reg);


#endif
