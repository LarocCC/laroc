#ifndef LAROC_RISCV_OPERAND_H
#define LAROC_RISCV_OPERAND_H

#include <stdbool.h>

#include "typedef.h"
#include "riscv/reg.h"

typedef enum RegState {
  REG_USE = 0,
  REG_DEFINE = 1 << 0,
  REG_KILL = 1 << 1,
  REG_DEAD = 1 << 2,
  REG_UNDEF = 1 << 3,
} RegState;

typedef enum OperandKind {
  RV_OP_INVAL,
  RV_OP_REG,
  RV_OP_IMM,
  RV_OP_SYM,
} OperandKind;

struct Operand {
  OperandKind kind;

  Reg reg;
  RegState regState;

  int imm;
  const char *sym;
};

Operand *newOperandReg(Reg reg, RegState state);
Operand *newOperandVirtReg(int reg, RegState state);
Operand *newOperandFrameObj(int id, RegState state);
Operand *newOperandImm(int imm);

void printOperand(Operand *op, bool debug);

#endif
