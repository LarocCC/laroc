#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ir/block.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"

Value *newValueVoid() {
  Value *x = calloc(1, sizeof(Value));
  x->ty = newIRType(IR_VOID);
  return x;
}

Value *newValueVar(IRFunc *func, IRType *ty) {
  Value *x = calloc(1, sizeof(Value));
  x->kind = IR_VAL_VAR;
  x->ty = ty;
  x->id = ++func->valueCount;
  return x;
}

Value *newValueImm(IRType *ty, int imm) {
  Value *x = calloc(1, sizeof(Value));
  x->kind = IR_VAL_IMM;
  x->ty = ty;
  x->imm = imm;
  return x;
}

Value *newValueBlock(IRBlock *block) {
  Value *x = calloc(1, sizeof(Value));
  x->kind = IR_VAL_BLOCK;
  x->ty = newIRType(IR_BLOCK);
  x->block = block;
  return x;
}

Value *newValueDAGNode(IRInst *inst) {
  Value *x = calloc(1, sizeof(Value));
  x->kind = IR_VAL_DAG_NODE;
  x->ty = inst->dst->ty;
  x->id = inst->dst->id;
  x->inst = inst;
  return x;
}

void printValue(Value *v) {
  switch (v->kind) {
  case IR_VOID:
    printf("void");
    return;

  case IR_VAL_VAR:
    printIRType(v->ty);
    printf(" %%%d", v->id);
    return;

  case IR_VAL_IMM:
    printf("%d", v->imm);
    return;

  case IR_VAL_BLOCK:
    printf(".B%d", v->block->id);
    return;

  case IR_VAL_DAG_NODE:
    printf("(");
    printIRInst(v->inst, false);
    printf(")");
    return;

  default:
    assert(false);
  }
}
