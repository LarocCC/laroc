#ifndef LAROC_CODEGEN_OPERAND_H
#define LAROC_CODEGEN_OPERAND_H

#include "typedef.h"
#include "codegen/reg.h"

typedef enum OperandKind {
  RV_OP_INVAL,
  RV_OP_REG,
  RV_OP_VIRT_REG,
  RV_OP_IMM,
  RV_OP_FRAME_OBJ,
  RV_OP_SYM,
} OperandKind;

struct Operand {
  OperandKind kind;

  Reg reg;
  int virtReg;
  int imm;
  int frameObjId;
  const char *sym;
};

Operand *newOperandReg(Reg reg);
Operand *newOperandVirtReg(int reg);
Operand *newOperandImm(int imm);
Operand *newOperandFrameObj(int id);

void printOperand(Operand *op);

#endif
