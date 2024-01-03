#include <assert.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/objfile.h"
#include "ir/ir.h"

RVFunc *newRVFunc(IRFunc *irFunc) {
  RVFunc *rvFunc = calloc(1, sizeof(RVFunc));
  rvFunc->name = irFunc->name;

  for (int i = 0; i < arrlen(irFunc->allocas); i++)
    funcAddFrameObjectFromAlloca(rvFunc, irFunc->allocas[i]);

  rvFunc->blockCount = irFunc->blockCount;

  return rvFunc;
}

FrameObject *newFrameObject(RVFunc *func, int size, int align) {
  FrameObject *obj = calloc(1, sizeof(FrameObject));
  obj->id = arrlen(func->frameObjs) + 1;
  obj->size = size;
  obj->align = align;
  return obj;
}

FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca) {
  assert(alloca->kind == IR_ALLOCA);

  assert(alloca->src1 != NULL && alloca->src1->kind == IR_VAL_IMM);
  assert(alloca->src1->ty != NULL && alloca->src1->ty->kind == IR_I32);
  int size = alloca->src1->imm;

  assert(alloca->src2 != NULL && alloca->src2->kind == IR_VAL_IMM);
  assert(alloca->src2->ty != NULL && alloca->src2->ty->kind == IR_I32);
  int align = alloca->src1->imm;

  FrameObject *obj = newFrameObject(func, size, align);
  arrput(func->frameObjs, obj);
  return obj;
}

RVBlock *newRVBlock(IRBlock *irBlock) {
  RVBlock *rvBlock = calloc(1, sizeof(RVBlock));
  rvBlock->id = irBlock->id;
  return rvBlock;
}
