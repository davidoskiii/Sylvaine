#ifndef cluminique_codegen_h
#define cluminique_codegen_h

#include "../ast/ast.h"

void freeRegisters();
void generateAssemblyPreamble();
void generateAssemblyPostamble();

int generateLoadInteger(int value);
int generateAddition(int reg1, int reg2);
int generateSubtraction(int reg1, int reg2);
int generateMultiplication(int reg1, int reg2);
int generateDivision(int reg1, int reg2);
int generateEqualComparison(int reg1, int reg2);
int generateNotEqualComparison(int reg1, int reg2);
int generateLessThanComparison(int reg1, int reg2);
int generateGreaterThanComparison(int reg1, int reg2);
int generateLessThanOrEqualComparison(int reg1, int reg2);
int generateGreaterThanOrEqualComparison(int reg1, int reg2);

void generatePrintInteger(int reg);

int loadGlobalSymbol(char* identifier);
int storeGlobalSymbol(int reg, char* identifier);
void generateGlobalSymbol(char* symbol);

#endif
