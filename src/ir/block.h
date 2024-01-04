#ifndef LAROC_IR_BLOCK_H
#define LAROC_IR_BLOCK_H

#include "typedef.h"

struct IRBlock {
  int id;
  IRBlock **preds, **succs;

  IRInst *instHead, *instTail;

  int lastVisitID;
};

IRBlock *newIRBlock(IRFunc *func);

void printIRBlock(IRBlock *blk);

#endif
