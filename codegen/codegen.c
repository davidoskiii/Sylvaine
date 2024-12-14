#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"
#include "../common.h"
#include "../ast/ast.h"
#include "../symbol/symbol.h"
#include "../misc/misc.h"

static bool registerAvailability[4];
static char* registerNames[4] = { "%r8", "%r9", "%r10", "%r11" };
static char* bregisterNames[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char* dregisterNames[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };

void freeRegisters() {
  for (int i = 0; i < 4; i++) {
    registerAvailability[i] = true;
  }
}

static int allocateRegister() {
  for (int i = 0; i < 4; i++) {
    if (registerAvailability[i]) {
      registerAvailability[i] = false;
      return i;
    }
  }
  fprintf(stderr, "Error: Out of registers!\n");
  exit(1);
}

static void freeRegister(int reg) {
  if (registerAvailability[reg] != false) {
    fprintf(stderr, "Error: Attempt to free already free register %d\n", reg);
    exit(1);
  }
  registerAvailability[reg] = true;
}

static int primitiveSize[] = { 0, 0, 1, 4, 8 };

int getPrimitiveSize(int type) {
  if (type < PRIMITIVE_NONE || type > PRIMITIVE_LONG_POINTER)
    fatal("Bad type in cgprimsize()");
  return primitiveSize[type];
}

void generateAssemblyPreamble() {
  freeRegisters();
  fputs("\t.text\n", compiler->fileO);
}

void generateAssemblyPostamble() {
}

void generateFunctionPreamble(int id) {
  char* name = globalSymbolTable[id].name;
  fprintf(compiler->fileO,
    "\t.text\n"
    "\t.globl\t%s\n"
    "\t.type\t%s, @function\n"
    "%s:\n"
    "\tpushq\t%%rbp\n"
    "\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

void generateFunctionPostamble(int id) {
  generateLabel(globalSymbolTable[id].endLabel);
  fputs("\tpopq %rbp\n" "\tret\n", compiler->fileO);
}

int generateLoadInteger(int value) {
  int reg = allocateRegister();
  fprintf(compiler->fileO, "\tmovq\t$%d, %s\n", value, registerNames[reg]);
  return reg;
}

int generateAddition(int reg1, int reg2) {
  fprintf(compiler->fileO, "\taddq\t%s, %s\n", registerNames[reg1], registerNames[reg2]);
  freeRegister(reg1);
  return reg2;
}

int generateSubtraction(int reg1, int reg2) {
  fprintf(compiler->fileO, "\tsubq\t%s, %s\n", registerNames[reg2], registerNames[reg1]);
  freeRegister(reg2);
  return reg1;
}

int generateMultiplication(int reg1, int reg2) {
  fprintf(compiler->fileO, "\timulq\t%s, %s\n", registerNames[reg1], registerNames[reg2]);
  freeRegister(reg1);
  return reg2;
}

int generateDivision(int reg1, int reg2) {
  fprintf(compiler->fileO, "\tmovq\t%s, %%rax\n", registerNames[reg1]);
  fprintf(compiler->fileO, "\tcqo\n");
  fprintf(compiler->fileO, "\tidivq\t%s\n", registerNames[reg2]);
  fprintf(compiler->fileO, "\tmovq\t%%rax, %s\n", registerNames[reg1]);
  freeRegister(reg2);
  return reg1;
}

static int compareRegisters(int reg1, int reg2, char* condition) {
  fprintf(compiler->fileO, "\tcmpq\t%s, %s\n", registerNames[reg2], registerNames[reg1]);
  fprintf(compiler->fileO, "\t%s\t%s\n", condition, bregisterNames[reg2]);
  fprintf(compiler->fileO, "\tmovzbq\t%s, %s\n", bregisterNames[reg2], registerNames[reg2]);
  freeRegister(reg1);
  return reg2;
}

static int compareAndJumpRegisters(int reg1, int reg2, char* condition, int label) {
  fprintf(compiler->fileO, "\tcmpq\t%s, %s\n", registerNames[reg2], registerNames[reg1]);
  fprintf(compiler->fileO, "\t%s\tL%d\n", condition, label);
  freeRegisters();
  return NOREG;
}

int generateEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "sete");
}

int generateEqualComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "jne", label);
}

int generateNotEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setne");
}

int generateNotEqualComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "je", label);
}

int generateLessThanComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setl");
}

int generateLessThanComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "jge", label);
}

int generateGreaterThanComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setg");
}

int generateGreaterThanComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "jle", label);
}

int generateLessThanOrEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setle");
}

int generateLessThanOrEqualComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "jg", label);
}

int generateGreaterThanOrEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setge");
}

int generateGreaterThanOrEqualComparisonAndJump(int reg1, int reg2, int label) {
  return compareAndJumpRegisters(reg1, reg2, "jl", label);
}

int compareAndSet(int ASTop, int reg1, int reg2) {
  switch (ASTop) {
    case AST_EQ:
      return generateEqualComparison(reg1, reg2);
    case AST_NE:
      return generateNotEqualComparison(reg1, reg2);
    case AST_LT:
      return generateLessThanComparison(reg1, reg2);
    case AST_GT:
      return generateGreaterThanComparison(reg1, reg2);
    case AST_LE:
      return generateLessThanOrEqualComparison(reg1, reg2);
    case AST_GE:
      return generateGreaterThanOrEqualComparison(reg1, reg2);
    default:
      fatal("Bad AST operation in compareAndSet()");
  }
}

int compareAndJump(int ASTop, int reg1, int reg2, int label) {
  switch (ASTop) {
    case AST_EQ:
      return generateEqualComparisonAndJump(reg1, reg2, label);
    case AST_NE:
      return generateNotEqualComparisonAndJump(reg1, reg2, label);
    case AST_LT:
      return generateLessThanComparisonAndJump(reg1, reg2, label);
    case AST_GT:
      return generateGreaterThanComparisonAndJump(reg1, reg2, label);
    case AST_LE:
      return generateLessThanOrEqualComparisonAndJump(reg1, reg2, label);
    case AST_GE:
      return generateGreaterThanOrEqualComparisonAndJump(reg1, reg2, label);
    default:
      fatal("Bad AST operation in compareAndJump()");
  }
}

void generatePrintInteger(int reg) {
  fprintf(compiler->fileO, "\tmovq\t%s, %%rdi\n", registerNames[reg]);
  fprintf(compiler->fileO, "\tcall\tprintint\n");
  freeRegister(reg);
}

int loadGlobalSymbol(int id) {
  int reg = allocateRegister();
  switch (globalSymbolTable[id].type) {
    case PRIMITIVE_CHAR:
      fprintf(compiler->fileO, "\tmovzbq\t%s(\%%rip), %s\n", globalSymbolTable[id].name,
	      bregisterNames[reg]);
      break;
    case PRIMITIVE_INT:
      fprintf(compiler->fileO, "\tmovzbl\t%s(\%%rip), %s\n", globalSymbolTable[id].name,
	      dregisterNames[reg]);
      break;
    case PRIMITIVE_LONG:
    case PRIMITIVE_CHAR_POINTER:
    case PRIMITIVE_INT_POINTER:
    case PRIMITIVE_LONG_POINTER:
      fprintf(compiler->fileO, "\tmovq\t%s(\%%rip), %s\n", globalSymbolTable[id].name, registerNames[reg]);
      break;
    default:
      fatald("Bad type in cgloadglob", globalSymbolTable[id].type);
  }
  return reg;
}

int storeGlobalSymbol(int reg, int id) {
  switch (globalSymbolTable[id].type) {
    case PRIMITIVE_CHAR:
      fprintf(compiler->fileO, "\tmovb\t%s, %s(\%%rip)\n", bregisterNames[reg],
	      globalSymbolTable[id].name);
      break;
    case PRIMITIVE_INT:
      fprintf(compiler->fileO, "\tmovl\t%s, %s(\%%rip)\n", dregisterNames[reg],
	      globalSymbolTable[id].name);
      break;
    case PRIMITIVE_LONG:
    case PRIMITIVE_CHAR_POINTER:
    case PRIMITIVE_INT_POINTER:
    case PRIMITIVE_LONG_POINTER:
      fprintf(compiler->fileO, "\tmovq\t%s, %s(\%%rip)\n", registerNames[reg], globalSymbolTable[id].name);
      break;
    default:
      fatald("Bad type in cgstoreglob", globalSymbolTable[id].type);
  }
  return reg;
}


void generateGlobalSymbol(int id) {
  int typesize;
  typesize = getPrimitiveSize(globalSymbolTable[id].type);

  fprintf(compiler->fileO, "\t.comm\t%s,%d,%d\n", globalSymbolTable[id].name, typesize, typesize);
}

void generateLabel(int label) {
  fprintf(compiler->fileO, "L%d:\n", label);
}

void generateJump(int label) {
  fprintf(compiler->fileO, "\tjmp\tL%d\n", label);
}

int generateWiden(int reg, PrimitiveTypes oldType, PrimitiveTypes newType) {
  return reg;
}

void generateReturn(int reg, int id) {
  switch (globalSymbolTable[id].type) {
    case PRIMITIVE_CHAR:
      fprintf(compiler->fileO, "\tmovzbl\t%s, %%eax\n", bregisterNames[reg]);
      break;
    case PRIMITIVE_INT:
      fprintf(compiler->fileO, "\tmovl\t%s, %%eax\n", dregisterNames[reg]);
      break;
    case PRIMITIVE_LONG:
      fprintf(compiler->fileO, "\tmovq\t%s, %%rax\n", registerNames[reg]);
      break;
    default:
      fatald("Bad function type in cgreturn:", globalSymbolTable[id].type);
  }
  generateJump(globalSymbolTable[id].endLabel);
}

int generateCall(int reg, int id) {
  int outr = allocateRegister();
  fprintf(compiler->fileO, "\tmovq\t%s, %%rdi\n", registerNames[reg]);
  fprintf(compiler->fileO, "\tcall\t%s\n", globalSymbolTable[id].name);
  fprintf(compiler->fileO, "\tmovq\t%%rax, %s\n", registerNames[outr]);
  freeRegister(reg);
  return outr;
}

int generateAddress(int id) {
  int reg = allocateRegister();

  fprintf(compiler->fileO, "\tleaq\t%s(%%rip), %s\n", globalSymbolTable[id].name, registerNames[reg]);
  return reg;
}

int generateDereference(int reg, PrimitiveTypes type) {
  switch (type) {
    case PRIMITIVE_CHAR_POINTER:
      fprintf(compiler->fileO, "\tmovzbq\t(%s), %s\n", registerNames[reg], registerNames[reg]);
      break;
    case PRIMITIVE_INT_POINTER:
    case PRIMITIVE_LONG_POINTER:
      fprintf(compiler->fileO, "\tmovq\t(%s), %s\n", registerNames[reg], registerNames[reg]);
      break;
  }
  return reg;
}
