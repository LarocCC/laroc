#ifndef LAROC_IR_BLOCK_H
#define LAROC_IR_BLOCK_H

#include "typedef.h"

/// An IR block.
struct IRBlock {
  int id;
  /// Predecessor and successor blocks.
  IRBlock **preds, **succs;

  /// Head and tail sentinel nodes for the linked list storing instruction in
  /// this block. If the block do contains instruction, instHead->next and
  /// instTail->prev in the actual first and last instruction of the block.
  IRInst *instHead, *instTail;

  int lastVisitID;
};

IRBlock *newIRBlock(IRFunc *func);

void printIRBlock(IRBlock *blk);

#endif
