#ifndef LAROC_RISCV_BLOCK_H
#define LAROC_RISCV_BLOCK_H

#include "typedef.h"

struct RVBlock {
  int id;
  RVBlock **preds, **succs;

  RVInst *instHead, *instTail;

  int lastVisitID;
};

RVBlock *newRVBlock(IRBlock *irBlock);

void printRVBlock(RVBlock *block);

#endif
