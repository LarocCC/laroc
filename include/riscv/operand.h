#ifndef LAROC_RISCV_OPERAND_H
#define LAROC_RISCV_OPERAND_H

#include <stdbool.h>

#include "typedef.h"
#include "riscv/reg.h"

typedef enum RegState {
  REG_USE = 0,
  /// Define the value of a register.
  REG_DEFINE = 1 << 0,
  /// Last use of a register.
  REG_KILL = 1 << 1,
  /// Define but the value is never used later.
  REG_DEAD = 1 << 2,
  /// Implicit define or use the register. The operand will not be printed
  /// unless printing debug information.
  REG_IMPLICIT = 1 << 3,
} RegState;

typedef enum OperandKind {
  RV_OP_INVAL, // Invalid operand
  RV_OP_REG,   // Register
  RV_OP_IMM,   // Immediate
  RV_OP_BLOCK, // Block
  RV_OP_MEM,   // Memory
  RV_OP_SYM,   // Symbol
} OperandKind;

/// An operand in a RISC-V instruction.
struct Operand {
  OperandKind kind;

  Reg reg;
  RegState regState;

  int imm;
  int blockID;
  const char *sym;
};

Operand *newOperandReg(Reg reg, RegState state);
Operand *newOperandVirtReg(int reg, RegState state);
Operand *newOperandFrameObj(int id, RegState state);
Operand *newOperandImm(int imm);
Operand *newOperandMem(Reg base, int offset);
Operand *newOperandBlock(int id);

void printOperand(Operand *op, bool debug);

#endif
