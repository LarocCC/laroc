#ifndef LAROC_RISCV_INST_H
#define LAROC_RISCV_INST_H

#include <stdbool.h>

#include "typedef.h"
#include "riscv/operand.h"
#include "riscv/reg.h"

typedef enum RVInstKind {
  RV_ILLEGAL, // Illegal Instruction

  RV_JALR, // jalr rd, imm(rs1)
  RV_LW,   // lw   rd, imm(rs1)
  RV_SW,   // sw   rd, imm(rs1)
  RV_ADDI, // addi rd, rs1, imm
  RV_ADD,  // add  rd, rs1, rs2
  RV_SUB,  // sub  rd, rs1, rs2

  RV_LI,  // li  rd, imm
  RV_MV,  // mv  rd, rs
  RV_J,   // j   imm
  RV_RET, // ret
} RVInstKind;

struct RVInst {
  RVInstKind kind;

  Operand **operands;

  RVInst *prev, *next;
};

RVInst *newRVInst(RVInstKind kind);

void rvInstAddReg(RVInst *inst, Reg reg, RegState state);
void rvInstAddVirtReg(RVInst *inst, int reg, RegState state);
void rvInstAddFrameObj(RVInst *inst, int id, RegState state);
void rvInstAddImm(RVInst *inst, int imm);
void rvInstAddBlock(RVInst *inst, int blockID);

void rvBlockAddInst(RVBlock *block, RVInst *inst);

void printRVInst(RVInst *inst, bool debug);

#endif
