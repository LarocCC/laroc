#ifndef LAROC_RISCV_INST_H
#define LAROC_RISCV_INST_H

#include <stdbool.h>

#include "typedef.h"
#include "riscv/operand.h"
#include "riscv/reg.h"

typedef enum RVInstKind {
  // Illegal Instruction
  RV_ILLEGAL,

  // RV32I Base Instruction Set
  RV_LUI,   // lui   rd, imm
  RV_AUIPC, // auipc rd, imm

  RV_JAL,  // jal  rd, imm
  RV_JALR, // jalr rd, imm(rs1)

  RV_BEQ,  // beq  rs1, rs2, imm
  RV_BNE,  // bne  rs1, rs2, imm
  RV_BLT,  // blt  rs1, rs2, imm
  RV_BGE,  // bge  rs1, rs2, imm
  RV_BLTU, // bltu rs1, rs2, imm
  RV_BGEU, // bgeu rs1, rs2, imm

  RV_LB,  // lb  rd, imm(rs1)
  RV_LH,  // lh  rd, imm(rs1)
  RV_LW,  // lw  rd, imm(rs1)
  RV_LBU, // lbu rd, imm(rs1)
  RV_LHU, // lhu rd, imm(rs1)
  RV_SB,  // sb  rd, imm(rs1)
  RV_SH,  // sh  rd, imm(rs1)
  RV_SW,  // sw  rd, imm(rs1)

  RV_ADDI,  // addi  rd, rs1, imm
  RV_SLTI,  // slti  rd, rs1, imm
  RV_SLTIU, // sltiu rd, rs1, imm
  RV_ORI,   // ori   rd, rs1, imm
  RV_XORI,  // xori  rd, rs1, imm
  RV_ANDI,  // andi  rd, rs1, imm
  RV_SLLI,  // slli  rd, rs1, shamt
  RV_SRLI,  // srli  rd, rs1, shamt
  RV_SRAI,  // srai  rd, rs1, shamt

  RV_ADD,  // add  rd, rs1, rs2
  RV_SUB,  // sub  rd, rs1, rs2
  RV_SLL,  // sll  rd, rs1, rs2
  RV_SLT,  // slt  rd, rs1, rs2
  RV_SLTU, // sltu rd, rs1, rs2
  RV_XOR,  // xor  rd, rs1, rs2
  RV_SRL,  // srl  rd, rs1, rs2
  RV_SRA,  // sra  rd, rs1, rs2
  RV_OR,   // or   rd, rs1, rs2
  RV_AND,  // and  rd, rs1, rs2

  RV_FENCE, // fence

  RV_ECALL,  // ecall
  RV_EBREAK, // ebreak

  // RISC-V pseudoinstructions
  RV_NOP,  // nop
  RV_LI,   // li   rd, imm
  RV_MV,   // mv   rd, rs1
  RV_NOT,  // not  rd, rs1
  RV_NEG,  // neg  rd, rs1
  RV_NEGW, // negw rd, rs1
  RV_SEQZ, // seqz rd, rs1
  RV_SNEZ, // snez rd, rs1
  RV_SLTZ, // sltz rd, rs1
  RV_SGTZ, // sgtz rd, rs1

  RV_BEQZ, // beqz rs1, imm
  RV_BNEZ, // bnez rs1, imm
  RV_BLEZ, // blez rs1, imm
  RV_BGEZ, // bgez rs1, imm
  RV_BLTZ, // bltz rs1, imm
  RV_BGTZ, // bgtz rs1, imm

  RV_BGT,  // bgt  rs1, rs2, imm
  RV_BLE,  // ble  rs1, rs2, imm
  RV_BGTU, // bgtu rs1, rs2, imm
  RV_BLEU, // bleu rs1, rs2, imm

  RV_J,    // j    imm
  RV_JR,   // jr   rs1
  RV_RET,  // ret
  RV_CALL, // call imm
  RV_TAIL, // tail imm

  RV_INST_KIND_LEN,
} RVInstKind;

extern const char *rvInstKindStr[RV_INST_KIND_LEN];

/// A RISC-V instruction.
struct RVInst {
  RVInstKind kind;

  Operand **operands;

  /// The previous and next instruction in current block.
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
