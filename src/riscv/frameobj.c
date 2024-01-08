#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"
#include "riscv/frameobj.h"
#include "riscv/func.h"

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
  assert(src1 && src1->kind == IR_VAL_IMM);
  int size = src1->imm;

  Value *src2 = alloca->srcs[1];
  assert(src2 && src2->kind == IR_VAL_IMM);
  int align = src1->imm;

  FrameObject *obj = newFrameObject(alloca->dst->id, size, align);
  arrput(func->frameObjs, obj);
  return obj;
}

void printFrameObject(FrameObject *frameObj) {
  printf("# {%%F%d} = { size:%d, align:%d }\n", frameObj->id,
         frameObj->size, frameObj->align);
}
