#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "codegen/inst.h"
#include "codegen/objfile.h"
#include "ir/ir.h"

void printObjectFile(ObjectFile *objFile) {
  for (int i = 0; i < arrlen(objFile->funcs); i++) {
    if (i != 0)
      printf("\n");
    printRVFunc(objFile->funcs[i]);
  }
}

RVFunc *newRVFunc(IRFunc *irFunc) {
  RVFunc *rvFunc = calloc(1, sizeof(RVFunc));
  rvFunc->name = irFunc->name;

  for (int i = 0; i < arrlen(irFunc->allocas); i++)
    funcAddFrameObjectFromAlloca(rvFunc, irFunc->allocas[i]);

  rvFunc->blockCount = irFunc->blockCount;
  arrput(rvFunc->blocks, NULL);

  return rvFunc;
}

void printRVFunc(RVFunc *func) {
  printf("%s:\n", func->name);

  printf("# entry = .B%d\n", func->entry->id);
  printf("# exits =");
  for (int i = 0; i < arrlen(func->exits); i++)
    printf(" .B%d", func->exits[i]->id);
  printf("\n");

  for (int i = 0; i < arrlen(func->frameObjs); i++)
    printFrameObject(func->frameObjs[i]);

  for (int i = 1; i <= func->blockCount; i++)
    printRVBlock(func->blocks[i]);
}

FrameObject *newFrameObject(int id, int size, int align) {
  FrameObject *obj = calloc(1, sizeof(FrameObject));
  obj->id = id;
  obj->size = size;
  obj->align = align;
  return obj;
}

FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca) {
  assert(alloca->kind == IR_ALLOCA);

  Value *src1 = alloca->srcs[0];
  assert(src1 != NULL && src1->kind == IR_VAL_IMM);
  assert(src1->ty != NULL && src1->ty->kind == IR_I32);
  int size = src1->imm;

  Value *src2 = alloca->srcs[1];
  assert(src2 != NULL && src2->kind == IR_VAL_IMM);
  assert(src2->ty != NULL && src2->ty->kind == IR_I32);
  int align = src1->imm;

  FrameObject *obj = newFrameObject(alloca->dst->id, size, align);
  arrput(func->frameObjs, obj);
  return obj;
}

void printFrameObject(FrameObject *frameObj) {
  printf("# frameobj:%%%d = { size:%d, align:%d }\n", frameObj->id,
         frameObj->size, frameObj->align);
}

RVBlock *newRVBlock(IRBlock *irBlock) {
  RVBlock *blk = calloc(1, sizeof(RVBlock));
  blk->id = irBlock->id;

  blk->instHead = newRVInst(RV_ILLEGAL);
  blk->instTail = newRVInst(RV_ILLEGAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;

  return blk;
}

void rvBlockAddInst(RVBlock *block, RVInst *inst) {
  inst->prev = block->instTail->prev;
  inst->next = block->instTail;
  inst->prev->next = inst;
  inst->next->prev = inst;
}

void printRVBlock(RVBlock *block) {
  printf(".B%d:\n", block->id);

  for (RVInst *inst = block->instHead->next; inst != block->instTail;
       inst = inst->next) {
    printRVInst(inst);
  }
}
