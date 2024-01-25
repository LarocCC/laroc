#ifndef LAROC_RISCV_FRAMEOBJ_H
#define LAROC_RISCV_FRAMEOBJ_H

#include <stdio.h>

#include "typedef.h"

/// A object in stack frame.
struct FrameObject {
  int id;
  int size, align;

  int offset;
};

FrameObject *newFrameObject(int id, int size, int align);
FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca);

void printFrameObject(FILE *fp, FrameObject *frameObj);

#endif
