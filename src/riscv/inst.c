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

void rvInstAddImm(RVInst *inst, int imm) {
  Operand *op = newOperandImm(imm);
  arrput(inst->operands, op);
}

void rvBlockAddInst(RVBlock *block, RVInst *inst) {
  inst->prev = block->instTail->prev;
  inst->next = block->instTail;
  inst->prev->next = inst;
  inst->next->prev = inst;
}

void printRVInst(RVInst *inst, bool debug) {
  printf("\t");
  switch (inst->kind) {
  case RV_JALR:
    printf("jalr");
    break;
  case RV_LW:
    printf("lw");
    break;
  case RV_SW:
    printf("sw");
    break;
  case RV_ADDI:
    printf("addi");
    break;
  case RV_ADD:
    printf("add");
    break;
  case RV_SUB:
    printf("sub");
    break;
  case RV_LI:
    printf("li");
    break;
  case RV_MV:
    printf("mv");
    break;
  case RV_J:
    printf("j");
    break;
  case RV_RET:
    printf("ret");
    break;
  default:
    assert(false);
  }

  for (int i = 0; i < arrlen(inst->operands); i++) {
    if (i == 0)
      printf("\t");
    else
      printf(", ");
    printOperand(inst->operands[i], debug);
  }
  printf("\n");
}
