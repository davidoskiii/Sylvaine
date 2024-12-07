#ifndef cluminique_codegen_h
#define cluminique_codegen_h

#include "../ast/ast.h"

void freeRegisters();
void generateAssemblyPreamble();
void generateFunctionPreamble(char* name);
void generateFunctionPostamble();

int generateLoadInteger(int value);
int generateAddition(int reg1, int reg2);
int generateSubtraction(int reg1, int reg2);
int generateMultiplication(int reg1, int reg2);
int generateDivision(int reg1, int reg2);

int compareAndSet(int ASTop, int reg1, int reg2);
int compareAndJump(int ASTop, int reg1, int reg2, int label);

void generateLabel(int label);
void generateJump(int label);

int generateWiden(int reg, PrimitiveTypes oldType, PrimitiveTypes newType);

void generatePrintInteger(int reg);

int loadGlobalSymbol(int id);
int storeGlobalSymbol(int reg, int id);
void generateGlobalSymbol(int id);

#endif
