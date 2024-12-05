#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"
#include "../common.h"
#include "../ast/ast.h"

static bool registerAvailability[4];
static char* registerNames[4] = { "%r8", "%r9", "%r10", "%r11" };
static char* bregisterNames[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };

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

void generateAssemblyPreamble() {
  freeRegisters();
  fputs(
    "\t.text\n"
    ".LC0:\n"
    "\t.string\t\"%d\\n\"\n"
    "printint:\n"
    "\tpushq\t%rbp\n"
    "\tmovq\t%rsp, %rbp\n"
    "\tsubq\t$16, %rsp\n"
    "\tmovl\t%edi, -4(%rbp)\n"
    "\tmovl\t-4(%rbp), %eax\n"
    "\tmovl\t%eax, %esi\n"
    "\tleaq\t.LC0(%rip), %rdi\n"
    "\tmovl\t$0, %eax\n"
    "\tcall\tprintf@PLT\n"
    "\tnop\n"
    "\tleave\n"
    "\tret\n"
    "\n"
    "\t.globl\tmain\n"
    "\t.type\tmain, @function\n"
    "main:\n"
    "\tpushq\t%rbp\n"
    "\tmovq\t%rsp, %rbp\n",
    compiler->fileO);
}

void generateAssemblyPostamble() {
  fputs(
    "\tmovl\t$0, %eax\n"
    "\tpopq\t%rbp\n"
    "\tret\n",
    compiler->fileO);
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

static int compareRegisters(int reg1, int reg2, char *condition) {
  fprintf(compiler->fileO, "\tcmpq\t%s, %s\n", registerNames[reg2], registerNames[reg1]);
  fprintf(compiler->fileO, "\t%s\t%s\n", condition, bregisterNames[reg2]);
  fprintf(compiler->fileO, "\tandq\t$255,%s\n", registerNames[reg2]);
  freeRegister(reg1);
  return reg2;
}

int generateEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "sete");
}

int generateNotEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setne");
}

int generateLessThanComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setl");
}

int generateGreaterThanComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setg");
}

int generateLessThanOrEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setle");
}

int generateGreaterThanOrEqualComparison(int reg1, int reg2) {
  return compareRegisters(reg1, reg2, "setge");
}

void generatePrintInteger(int reg) {
  fprintf(compiler->fileO, "\tmovq\t%s, %%rdi\n", registerNames[reg]);
  fprintf(compiler->fileO, "\tcall\tprintint\n");
  freeRegister(reg);
}

int loadGlobalSymbol(char* identifier) {
  int reg = allocateRegister();
  fprintf(compiler->fileO, "\tmovq\t%s(\%%rip), %s\n", identifier, registerNames[reg]);
  return reg;
}

int storeGlobalSymbol(int reg, char* identifier) {
  fprintf(compiler->fileO, "\tmovq\t%s, %s(\%%rip)\n", registerNames[reg], identifier);
  return reg;
}

void generateGlobalSymbol(char* symbol) {
  fprintf(compiler->fileO, "\t.comm\t%s,8,8\n", symbol);
}
