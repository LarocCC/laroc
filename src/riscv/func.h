#ifndef LAROC_RISCV_FUNC_H
#define LAROC_RISCV_FUNC_H

#include "typedef.h"

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

#endif
