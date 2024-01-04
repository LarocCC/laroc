#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/inst.h"
#include "codegen/objfile.h"

void printReg(Reg r) {
  switch (r) {
  case RV_ZERO:
    printf("zero");
    return;
  case RV_RA:
    printf("ra");
    return;
  case RV_SP:
    printf("sp");
    return;
  case RV_GP:
    printf("gp");
    return;
  case RV_TP:
    printf("tp");
    return;
  case RV_T0:
    printf("t0");
    return;
  case RV_T1:
    printf("t1");
    return;
  case RV_T2:
    printf("t2");
    return;
  case RV_S0:
    printf("s0");
    return;
  case RV_S1:
    printf("s1");
    return;
  case RV_A0:
    printf("a0");
    return;
  case RV_A1:
    printf("a1");
    return;
  case RV_A2:
    printf("a2");
    return;
  case RV_A3:
    printf("a3");
    return;
  case RV_A4:
    printf("a4");
    return;
  case RV_A5:
    printf("a5");
    return;
  case RV_A6:
    printf("a6");
    return;
  case RV_A7:
    printf("a7");
    return;
  case RV_S2:
    printf("s2");
    return;
  case RV_S3:
    printf("s3");
    return;
  case RV_S4:
    printf("s4");
    return;
  case RV_S5:
    printf("s5");
    return;
  case RV_S6:
    printf("s6");
    return;
  case RV_S7:
    printf("s7");
    return;
  case RV_S8:
    printf("s8");
    return;
  case RV_S9:
    printf("s9");
    return;
  case RV_S10:
    printf("s10");
    return;
  case RV_S11:
    printf("s11");
    return;
  case RV_T3:
    printf("t3");
    return;
  case RV_T4:
    printf("t4");
    return;
  case RV_T5:
    printf("t5");
    return;
  case RV_T6:
    printf("t6");
    return;
  default:
    assert(false);
  }
}

Operand *newOperandReg(Reg reg) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_REG;
  op->reg = reg;
  return op;
}

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

Operand *newOperandFrameObj(int id) {
  Operand *op = calloc(1, sizeof(Operand));
  op->kind = RV_OP_FRAME_OBJ;
  op->frameObjId = id;
  return op;
}

void printOperand(Operand *op) {
  switch (op->kind) {
  case RV_OP_REG:
    return printReg(op->reg);

  case RV_OP_VIRT_REG:
    printf("%%%d", op->virtReg);
    return;

  case RV_OP_IMM:
    printf("%d", op->imm);
    return;

  case RV_OP_FRAME_OBJ:
    printf("{frameobj:%%%d}", op->frameObjId);
    return;

  default:
    assert(false);
  }
}

RVInst *newRVInst(RVInstKind kind) {
  RVInst *inst = calloc(1, sizeof(RVInst));
  inst->kind = kind;
  return inst;
}

void printRVInst(RVInst *inst) {
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
    printOperand(inst->operands[i]);
  }
  printf("\n");
}
