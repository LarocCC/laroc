#ifndef LAROC_CODEGEN_MODULE_H
#define LAROC_CODEGEN_MODULE_H

#include "typedef.h"

struct ObjectFile {
  RVFunc **funcs;
};

struct RVFunc {
  const char *name;

  FrameObject **frameObjs;

  int blockCount;
  RVBlock **blocks;
  RVBlock *entry;
  RVBlock **exits;
};

RVFunc *newRVFunc(IRFunc *irFunc);

struct FrameObject {
  int id;
  int size, align;
};

FrameObject *newFrameObject(RVFunc *func, int size, int align);
FrameObject *funcAddFrameObjectFromAlloca(RVFunc *func, IRInst *alloca);

struct RVBlock {
  int id;
  IRBlock **precs, **succs;

  RVInst **insts;
};

RVBlock *newRVBlock(IRBlock *irBlock);

#endif
