#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "riscv/block.h"
#include "riscv/inst.h"
#include "riscv/reg.h"

RVBlock *newRVBlock(IRBlock *irBlock) {
  RVBlock *blk = calloc(1, sizeof(RVBlock));
  blk->id = irBlock->id;

  blk->instHead = newRVInst(RV_ILLEGAL);
  blk->instTail = newRVInst(RV_ILLEGAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;

  return blk;
}

void printRVBlock(RVBlock *block, bool debug) {
  printf(".B%d:\n", block->id);

  if (debug) {
    printf("# preds =");
    for (int i = 0; i < arrlen(block->preds); i++)
      printf(" .B%d", block->preds[i]->id);
    printf("\n# liveIns =");
    for (int i = 0; i < arrlen(block->liveIns); i++) {
      printf(" ");
      printReg(block->liveIns[i]);
    }
    printf("\n# gens =");
    for (int i = 0; i < arrlen(block->gens); i++) {
      printf(" ");
      printReg(block->gens[i]);
    }
    printf("\n# kills =");
    for (int i = 0; i < arrlen(block->kills); i++) {
      printf(" ");
      printReg(block->kills[i]);
    }
    printf("\n# liveOuts =");
    for (int i = 0; i < arrlen(block->liveOuts); i++) {
      printf(" ");
      printReg(block->liveOuts[i]);
    }
    printf("\n# succs =");
    for (int i = 0; i < arrlen(block->succs); i++)
      printf(" .B%d", block->succs[i]->id);
    printf("\n");
  }

  for (RVInst *inst = block->instHead->next; inst != block->instTail;
       inst = inst->next) {
    printRVInst(inst, debug);
  }
}
