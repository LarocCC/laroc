#ifndef LAROC_RISCV_INST_H
#define LAROC_RISCV_INST_H

#include "typedef.h"
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

void rvInstAddReg(RVInst *inst, Reg reg);
void rvInstAddVirtReg(RVInst *inst, int reg);
void rvInstAddImm(RVInst *inst, int imm);
void rvInstAddFrameObj(RVInst *inst, int id);

void rvBlockAddInst(RVBlock *block, RVInst *inst);

void printRVInst(RVInst *inst);

#endif
