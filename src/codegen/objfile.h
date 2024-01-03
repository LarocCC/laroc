#ifndef LAROC_CODEGEN_MODULE_H
#define LAROC_CODEGEN_MODULE_H

#include "typedef.h"

struct ObjectFile {
  RVFunc **funcs;
};

void printObjectFile(ObjectFile *objFile);

struct RVFunc {
  const char *name;

  FrameObject **frameObjs;

  int blockCount;
  RVBlock **blocks;
  RVBlock *entry;
  RVBlock **exits;
};

RVFunc *newRVFunc(IRFunc *irFunc);

void printRVFunc(RVFunc *func);

struct FrameObject {
  int id;
  int size, align;
};

FrameObject *newFrameObject(RVFunc *func, int size, int align);
FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca);

void printFrameObject(FrameObject *frameObj);

struct RVBlock {
  int id;
  RVBlock **precs, **succs;

  RVInst *instHead, *instTail;
};

RVBlock *newRVBlock(IRBlock *irBlock);

void printRVBlock(RVBlock *block);

#endif
