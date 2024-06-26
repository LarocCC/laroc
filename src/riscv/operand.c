#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
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

Operand *newOperandFrameObj(int id, RegState state) {
  return newOperandReg(FRAME_OBJ_ID_BEGIN + id, state);
}

Operand *newOperandImm(int imm) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_IMM;
  op->imm = imm;
  return op;
}

Operand *newOperandMem(Reg base, int offset) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_MEM;
  op->reg = base;
  op->imm = offset;
  return op;
}

Operand *newOperandBlock(int id) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_BLOCK;
  op->blockID = id;
  return op;
}

void printOperand(FILE *fp, Operand *op, bool debug) {
  switch (op->kind) {
  case RV_OP_REG:
    if (debug) {
      if (op->regState & REG_IMPLICIT)
        fprintf(fp, "implicit ");
      if (op->regState & REG_DEFINE)
        fprintf(fp, "def ");
      if (op->regState & REG_KILL)
        fprintf(fp, "kill ");
      if (op->regState & REG_DEAD)
        fprintf(fp, "dead ");
    }
    return printReg(fp, op->reg);

  case RV_OP_IMM:
    fprintf(fp, "%d", op->imm);
    return;

  case RV_OP_BLOCK:
    fprintf(fp, ".B%d", op->blockID);
    return;

  case RV_OP_MEM:
    fprintf(fp, "%d(", op->imm);
    printReg(fp, op->reg);
    fprintf(fp, ")");
    return;

  default:
    assert(false);
  }
}
