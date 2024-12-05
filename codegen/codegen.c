#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"
#include "../common.h"
#include "../ast/ast.h"

static bool registerAvailability[4];
static char* registerNames[4] = { "%r8", "%r9", "%r10", "%r11" };

void generateCode(ASTNode* node) {
  int reg;

  generateAssemblyPreamble();
  reg= generateAST(node);
  generatePrintInteger(reg);
  generateAssemblyPostamble();
}

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

void generatePrintInteger(int reg) {
  fprintf(compiler->fileO, "\tmovq\t%s, %%rdi\n", registerNames[reg]);
  fprintf(compiler->fileO, "\tcall\tprintint\n");
  freeRegister(reg);
}
