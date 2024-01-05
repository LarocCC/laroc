#ifndef LAROC_RISCV_BLOCK_H
#define LAROC_RISCV_BLOCK_H

#include <stdbool.h>

#include "typedef.h"
#include "riscv/reg.h"

struct RVBlock {
  int id;
  RVBlock **preds, **succs;

  RVInst *instHead, *instTail;

  Reg *liveIns, *liveOuts;

  int lastVisitID;
};

RVBlock *newRVBlock(IRBlock *irBlock);

void printRVBlock(RVBlock *block, bool debug);

#endif
