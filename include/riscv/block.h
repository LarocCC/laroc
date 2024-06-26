#ifndef LAROC_RISCV_BLOCK_H
#define LAROC_RISCV_BLOCK_H

#include <stdbool.h>
#include <stdio.h>

#include "typedef.h"
#include "riscv/reg.h"

/// A RISC-V block.
struct RVBlock {
  /// The function that this block belongs to.
  RVFunc *func;

  int id;
  /// Predecessor and successor blocks.
  RVBlock **preds, **succs;

  /// Head and tail sentinel nodes for the linked list storing instruction in
  /// this block. If the block do contains instruction, instHead->next and
  /// instTail->prev in the actual first and last instruction of the block.
  RVInst *instHead, *instTail;

  Reg *liveIns, *liveOuts;
  Reg *gens, *kills;

  int lastVisitID;
};

RVBlock *newRVBlock(RVFunc *func, IRBlock *irBlock);

void printRVBlock(FILE *fp, RVBlock *block, bool debug);

#endif
