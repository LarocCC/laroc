#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "riscv/block.h"
#include "riscv/inst.h"
#include "riscv/operand.h"

RVInst *newRVInst(RVInstKind kind) {
  RVInst *inst = calloc(1, sizeof(RVInst));
  inst->kind = kind;
  return inst;
}

void rvInstAddReg(RVInst *inst, Reg reg, RegState state) {
  Operand *op = newOperandReg(reg, state);
  arrput(inst->operands, op);
}

void rvInstAddVirtReg(RVInst *inst, int reg, RegState state) {
  Operand *op = newOperandVirtReg(reg, state);
  arrput(inst->operands, op);
}

void rvInstAddFrameObj(RVInst *inst, int id, RegState state) {
  Operand *op = newOperandFrameObj(id, state);
  arrput(inst->operands, op);
}

void rvInstAddBlock(RVInst *inst, int blockID) {
  Operand *op = newOperandBlock(blockID);
  arrput(inst->operands, op);
}

void rvInstAddImm(RVInst *inst, int imm) {
  Operand *op = newOperandImm(imm);
  arrput(inst->operands, op);
}

void rvInsertInstAfter(RVInst *pos, RVInst *inst) {
  inst->prev = pos;
  inst->next = pos->next;
  inst->next->prev = inst;
  inst->prev->next = inst;
}

void printRVInst(RVInst *inst, bool debug) {
  assert(rvInstKindStr[inst->kind]);
  printf("\t%s", rvInstKindStr[inst->kind]);

  for (int i = 0; i < arrlen(inst->operands); i++) {
    if (i == 0)
      printf("\t");
    else
      printf(", ");
    printOperand(inst->operands[i], debug);
  }
  printf("\n");
}

const char *rvInstKindStr[RV_INST_KIND_LEN] = {
    // RV32I Base Instruction Set
    [RV_LUI] = "lui",
    [RV_AUIPC] = "auipc",
    [RV_JAL] = "jal",
    [RV_JALR] = "jalr",
    [RV_BEQ] = "beq",
    [RV_BNE] = "bne",
    [RV_BLT] = "blt",
    [RV_BGE] = "bge",
    [RV_BLTU] = "bltu",
    [RV_BGEU] = "bgeu",
    [RV_LB] = "lb",
    [RV_LH] = "lh",
    [RV_LW] = "lw",
    [RV_LBU] = "lbu",
    [RV_LHU] = "lhu",
    [RV_SB] = "sb",
    [RV_SH] = "sh",
    [RV_SW] = "sw",
    [RV_ADDI] = "addi",
    [RV_SLTI] = "slti",
    [RV_SLTIU] = "sltiu",
    [RV_ORI] = "ori",
    [RV_XORI] = "xori",
    [RV_ANDI] = "andi",
    [RV_SLLI] = "slli",
    [RV_SRLI] = "srli",
    [RV_SRAI] = "srai",
    [RV_ADD] = "add",
    [RV_SUB] = "sub",
    [RV_SLL] = "sll",
    [RV_SLT] = "slt",
    [RV_SLTU] = "sltu",
    [RV_XOR] = "xor",
    [RV_SRL] = "srl",
    [RV_SRA] = "sra",
    [RV_OR] = "or",
    [RV_AND] = "and",
    [RV_FENCE] = "fence",
    [RV_ECALL] = "ecall",
    [RV_EBREAK] = "ebreak",

    // RV64I Base Instruction Set
    [RV_LWU] = "lwu",
    [RV_LD] = "ld",
    [RV_SD] = "sd",
    [RV_ADDIW] = "addiw",
    [RV_SLLIW] = "slliw",
    [RV_SRLIW] = "srliw",
    [RV_SRAIW] = "sraiw",
    [RV_ADDW] = "addw",
    [RV_SUBW] = "subw",
    [RV_SLLW] = "sllw",
    [RV_SRLW] = "srlw",
    [RV_SRAW] = "sraw",

    // RISC-V pseudoinstructions
    [RV_NOP] = "nop",
    [RV_LI] = "li",
    [RV_MV] = "mv",
    [RV_NOT] = "not",
    [RV_NEG] = "neg",
    [RV_NEGW] = "negw",
    [RV_SEQZ] = "seqz",
    [RV_SNEZ] = "snez",
    [RV_SLTZ] = "sltz",
    [RV_SGTZ] = "sgtz",
    [RV_BEQZ] = "beqz",
    [RV_BNEZ] = "bnez",
    [RV_BLEZ] = "blez",
    [RV_BGEZ] = "bgez",
    [RV_BLTZ] = "bltz",
    [RV_BGTZ] = "bgtz",
    [RV_BGT] = "bgt",
    [RV_BLE] = "ble",
    [RV_BGTU] = "bgtu",
    [RV_BLEU] = "bleu",
    [RV_J] = "j",
    [RV_JR] = "jr",
    [RV_RET] = "ret",
    [RV_CALL] = "call",
    [RV_TAIL] = "tail",
};
