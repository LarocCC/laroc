#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/inst.h"
#include "codegen/operand.h"

RVInst *newRVInst(RVInstKind kind) {
  RVInst *inst = calloc(1, sizeof(RVInst));
  inst->kind = kind;
  return inst;
}

void rvInstAddReg(RVInst *inst, Reg reg) {
  Operand *op = newOperandReg(reg);
  arrput(inst->operands, op);
}

void rvInstAddVirtReg(RVInst *inst, int reg) {
  Operand *op = newOperandVirtReg(reg);
  arrput(inst->operands, op);
}

void rvInstAddImm(RVInst *inst, int imm) {
  Operand *op = newOperandImm(imm);
  arrput(inst->operands, op);
}

void rvInstAddFrameObj(RVInst *inst, int id) {
  Operand *op = newOperandFrameObj(id);
  arrput(inst->operands, op);
}

void printRVInst(RVInst *inst) {
  printf("\t");
  switch (inst->kind) {
  case RV_JALR:
    printf("jalr");
    break;
  case RV_LW:
    printf("lw");
    break;
  case RV_SW:
    printf("sw");
    break;
  case RV_ADDI:
    printf("addi");
    break;
  case RV_ADD:
    printf("add");
    break;
  case RV_SUB:
    printf("sub");
    break;
  case RV_LI:
    printf("li");
    break;
  case RV_MV:
    printf("mv");
    break;
  case RV_J:
    printf("j");
    break;
  case RV_RET:
    printf("ret");
    break;
  default:
    assert(false);
  }

  for (int i = 0; i < arrlen(inst->operands); i++) {
    if (i == 0)
      printf("\t");
    else
      printf(", ");
    printOperand(inst->operands[i]);
  }
  printf("\n");
}
