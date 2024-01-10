#ifndef LAROC_RISCV_FRAMEOBJ_H
#define LAROC_RISCV_FRAMEOBJ_H

#include "typedef.h"

/// A object in stack frame.
struct FrameObject {
  int id;
  int size, align;
};

FrameObject *newFrameObject(int id, int size, int align);
FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca);

void printFrameObject(FrameObject *frameObj);

#endif
