#ifndef LAROC_RISCV_OPERAND_H
#define LAROC_RISCV_OPERAND_H

#include "typedef.h"
#include "riscv/reg.h"

typedef enum RegState {
  REG_USE = 0,
  REG_DEFINE = 1 << 0,
  REG_KILL = 1 << 1,
  REG_UNDEF = 1 << 2,
} RegState;

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
  RegState regState;

  int imm;
  int frameObjId;
  const char *sym;
};

Operand *newOperandReg(Reg reg, RegState state);
Operand *newOperandVirtReg(int reg, RegState state);
Operand *newOperandImm(int imm);
Operand *newOperandFrameObj(int id);

void printOperand(Operand *op);

#endif
