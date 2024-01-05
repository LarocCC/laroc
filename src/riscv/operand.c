#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "riscv/operand.h"
#include "riscv/reg.h"

Operand *newOperandReg(Reg reg, RegState state) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_REG;
  op->reg = reg;
  op->regState = state;
  return op;
}

Operand *newOperandVirtReg(int reg, RegState state) {
  return newOperandReg(VIRT_REG_BEGIN + reg, state);
}

Operand *newOperandImm(int imm) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_IMM;
  op->imm = imm;
  return op;
}

Operand *newOperandFrameObj(int id) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_FRAME_OBJ;
  op->frameObjId = id;
  return op;
}

void printOperand(Operand *op, bool debug) {
  switch (op->kind) {
  case RV_OP_REG:
    if (debug) {
      if (op->regState & REG_DEFINE)
        printf("def ");
      if (op->regState & REG_KILL)
        printf("kill ");
      if (op->regState & REG_DEAD)
        printf("dead ");
      if (op->regState & REG_UNDEF)
        printf("undef ");
    }
    return printReg(op->reg);

  case RV_OP_IMM:
    printf("%d", op->imm);
    return;

  case RV_OP_FRAME_OBJ:
    printf("{frameobj:%%%d}", op->frameObjId);
    return;

  default:
    assert(false);
  }
}
