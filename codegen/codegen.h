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

int compareAndSet(int ASTop, int reg1, int reg2);
int compareAndJump(int ASTop, int reg1, int reg2, int label);

void generateLabel(int label);
void generateJump(int label);

void generatePrintInteger(int reg);

int loadGlobalSymbol(char* identifier);
int storeGlobalSymbol(int reg, char* identifier);
void generateGlobalSymbol(char* symbol);

#endif
