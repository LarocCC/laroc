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
    printFunc(mod->funcs[i]);
  }
}

Func *newFunc(const char *name) {
  Func *func = calloc(1, sizeof(Func));
  func->name = name;
  return func;
}

void printFunc(Func *func) {
  printf("func ");
  printIRType(func->ret);
  printf(" @%s(", func->name);
  for (int i = 0; i < arrlen(func->args); i++) {
    if (i != 0)
      printf(", ");
    printValue(func->args[i]);
  }
  printf(") {\n");

  for (int i = 0; i < arrlen(func->allocas); i++)
    printIRInst(func->allocas[i]);

  printBlock(func->entry);

  printf("}\n");
}

Block *newBlock(Func *func) {
  Block *blk = calloc(1, sizeof(Block));
  blk->id = ++func->blockCount;
  return blk;
}

void printBlock(Block *blk) {
  printf(".B%d:\n", blk->id);
  for (int i = 0; i < arrlen(blk->insts); i++)
    printIRInst(blk->insts[i]);
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

void printIRInst(IRInst *inst) {
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
  printf("\n");
}

Value *newValueVoid() {
  Value *x = calloc(1, sizeof(Value));
  x->ty = newIRType(IR_VOID);
  return x;
}

Value *newValueVar(Func *func, IRType *ty) {
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

void printValue(Value *v) {
  printIRType(v->ty);
  if (v->ty->kind == IR_VOID)
    return;

  printf(" ");
  switch (v->kind) {
  case IR_VAL_VAR:
    printf("%%%d", v->id);
    return;

  case IR_VAL_IMM:
    printf("%lu", v->imm);
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
  default:
    assert(false);
  }
}
