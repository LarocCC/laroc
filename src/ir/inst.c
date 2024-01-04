#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/inst.h"
#include "ir/module.h"

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

  if (inst->dst != NULL) {
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

IRType *newIRType(IRTypeKind kind) {
  IRType *ty = calloc(1, sizeof(IRType));
  ty->kind = kind;
  return ty;
}

void printIRType(IRType *ty) {
  switch (ty->kind) {
  case IR_VOID:
    printf("void");
    return;
  case IR_PTR:
    printf("ptr");
    return;
  case IR_I8:
    printf("i8");
    return;
  case IR_I16:
    printf("i16");
    return;
  case IR_I32:
    printf("i32");
    return;
  case IR_I64:
    printf("i64");
    return;
  case IR_U8:
    printf("u8");
    return;
  case IR_U16:
    printf("u16");
    return;
  case IR_U32:
    printf("u32");
    return;
  case IR_U64:
    printf("u64");
    return;
  case IR_BLOCK:
    printf("block");
    return;
  default:
    assert(false);
  }
}
