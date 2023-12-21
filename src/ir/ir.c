#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

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

void printBlock(Block *blk) {
  for (int i = 0; i < arrlen(blk->insts); i++)
    printIRInst(blk->insts[i]);
}

IRInst *newIRInst(IRInstKind kind) {
  IRInst *inst = calloc(1, sizeof(IRInst));
  inst->kind = kind;
  return inst;
}

void printIRInst(IRInst *inst) {
  printf("  ");

  const char *str = NULL;
  bool printDst = false;
  bool printSrc1 = false, printSrc2 = false;

  switch (inst->kind) {
  case IR_ALLOCA:
    printValue(inst->dst);
    printf(" = alloca ");
    printIRType(inst->ty);
    printf("\n");
    return;

  case IR_LOAD:
    str = "load";
    printDst = printSrc1 = true;
    break;
  case IR_STORE:
    str = "store";
    printSrc1 = printSrc2 = true;
    break;
  case IR_ADD:
    str = "add";
    printDst = printSrc1 = printSrc2 = true;
    break;
  case IR_SUB:
    str = "sub";
    printDst = printSrc1 = printSrc2 = true;
    break;
  case IR_RET:
    str = "ret";
    printSrc1 = true;
    break;
  }
  assert(str != NULL);

  if (printDst) {
    printValue(inst->dst);
    printf(" = ");
  }
  printf("%s", str);
  if (printSrc1) {
    printf(" ");
    printValue(inst->src1);
    if (printSrc2) {
      printf(", ");
      printValue(inst->src2);
    }
  }
  printf("\n");
}

Value *newValueVoid() { return calloc(1, sizeof(Value)); }

Value *newValueVar(IRType *ty, const char *name) {
  static int id = 0;

  Value *x = calloc(1, sizeof(Value));
  x->kind = IR_VAL_VAR;
  x->ty = ty;
  x->name = name;
  x->id = ++id;
  return x;
}

void printValue(Value *v) {
  printIRType(v->ty);
  printf(" ");
  switch (v->kind) {
  case IR_VAL_VAR:
    printf("%%%d", v->id);
    if (v->name != NULL)
      printf(".%s", v->name);
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
