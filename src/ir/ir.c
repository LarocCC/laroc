#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/ir.h"

void printModule(Module *mod) {
  for (int i = 0; i < arrlen(mod->funcs); i++) {
    if (i != 0)
      printf("\n");
    printIRFunc(mod->funcs[i]);
  }
}

IRFunc *newIRFunc(const char *name) {
  IRFunc *func = calloc(1, sizeof(IRFunc));
  func->name = name;
  return func;
}

void printIRFunc(IRFunc *func) {
  printf("func ");
  printIRType(func->ret);
  printf(" @%s(", func->name);
  for (int i = 0; i < arrlen(func->args); i++) {
    if (i != 0)
      printf(", ");
    printValue(func->args[i]);
  }
  printf(") {\n");

  printf("; entry = .B%d\n", func->entry->id);
  printf("; exits =");
  for (int i = 0; i < arrlen(func->exits); i++)
    printf(" .B%d", func->exits[i]->id);
  printf("\n");

  for (int i = 0; i < arrlen(func->allocas); i++)
    printIRInst(func->allocas[i], true);

  for (int i = 0; i < func->blockCount; i++)
    printIRBlock(func->blocks[i]);

  printf("}\n");
}

IRBlock *newIRBlock(IRFunc *func) {
  IRBlock *blk = calloc(1, sizeof(IRBlock));
  arrput(func->blocks, blk);
  blk->id = ++func->blockCount;
  blk->instHead = newIRInst(IR_INVAL);
  blk->instTail = newIRInst(IR_INVAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;
  return blk;
}

void printIRBlock(IRBlock *blk) {
  printf("\n.B%d:\n", blk->id);

  printf("; precs =");
  for (int i = 0; i < arrlen(blk->precs); i++)
    printf(" .B%d", blk->precs[i]->id);
  printf("\n; succs =");
  for (int i = 0; i < arrlen(blk->succs); i++)
    printf(" .B%d", blk->succs[i]->id);
  printf("\n");

  for (IRInst *inst = blk->instHead->next; inst != blk->instTail;
       inst = inst->next)
    printIRInst(inst, true);
}

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

  if (inst->src1 != NULL) {
    printf(" ");
    printValue(inst->src1);

    if (inst->src2 != NULL) {
      printf(", ");
      printValue(inst->src2);
    }
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

Value *newValueImm(IRType *ty, uint64_t imm) {
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
    printIRType(v->ty);
    printf(" %lu", v->imm);
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
