#ifndef LAROC_RISCV_FUNC_H
#define LAROC_RISCV_FUNC_H

#include <stdbool.h>

#include "typedef.h"

struct RVFunc {
  const char *name;

  /// Frame objects used by this function.
  FrameObject **frameObjs;

  int blockCount;
  RVBlock **blocks;
  RVBlock *entry;
  RVBlock **exits;
};

RVFunc *newRVFunc(IRFunc *irFunc);

void printRVFunc(RVFunc *func, bool debug);

#endif
