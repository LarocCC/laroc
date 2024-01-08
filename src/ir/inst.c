#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"

void printIRInstKind(IRInstKind kind) {
  switch (kind) {
  case IR_ALLOCA:
    printf("alloca");
    break;
  case IR_LOAD:
    printf("load");
    break;
  case IR_STORE:
    printf("store");
    break;
  case IR_ADD:
    printf("add");
    break;
  case IR_SUB:
    printf("sub");
    break;
  case IR_LI:
    printf("li");
    break;
  case IR_BR:
    printf("br");
    break;
  case IR_J:
    printf("j");
    break;
  case IR_RET:
    printf("ret");
    break;
  default:
    assert(false);
  }
}

IRInst *newIRInst(IRInstKind kind) {
  IRInst *inst = calloc(1, sizeof(IRInst));
  inst->kind = kind;
  return inst;
}

void irBlockAddInst(IRBlock *blk, IRInst *inst) {
  inst->prev = blk->instTail->prev;
  inst->next = blk->instTail;
  inst->prev->next = inst;
  inst->next->prev = inst;
  inst->block = blk;
}

void irBlockRemoveInst(IRInst *inst) {
  inst->prev->next = inst->next;
  inst->next->prev = inst->prev;
  inst->prev = NULL;
  inst->next = NULL;
  inst->block = NULL;
}

void printIRInst(IRInst *inst, bool newLine) {
  if (newLine)
    printf("  ");

  if (inst->dst) {
    printValue(inst->dst);
    printf(" = ");
  }

  printIRInstKind(inst->kind);

  for (int i = 0; i < arrlen(inst->srcs); i++) {
    if (i != 0)
      printf(", ");
    else
      printf(" ");
    printValue(inst->srcs[i]);
  }

  if (newLine)
    printf("\n");
}
