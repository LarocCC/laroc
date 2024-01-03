#include <stdlib.h>

#include "typedef.h"
#include "codegen/inst.h"

Operand *newOperandVirtReg(int reg) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_VIRT_REG;
  op->virtReg = reg;
  return op;
}

Operand *newOperandImm(int imm) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_IMM;
  op->imm = imm;
  return op;
}

Operand *newOperandFrameObj(IRInst *alloca) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_FRAME_OBJ;
  op->frameObj = alloca;
  return op;
}

RVInst *newRVInst(RVInstKind kind) {
  RVInst *inst = calloc(1, sizeof(RVInst));
  inst->kind = kind;
  return inst;
}
