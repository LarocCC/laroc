#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "riscv/block.h"
#include "riscv/inst.h"

RVBlock *newRVBlock(IRBlock *irBlock) {
  RVBlock *blk = calloc(1, sizeof(RVBlock));
  blk->id = irBlock->id;

  blk->instHead = newRVInst(RV_ILLEGAL);
  blk->instTail = newRVInst(RV_ILLEGAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;

  return blk;
}

void printRVBlock(RVBlock *block) {
  printf(".B%d:\n", block->id);

  for (RVInst *inst = block->instHead->next; inst != block->instTail;
       inst = inst->next) {
    printRVInst(inst);
  }
}
