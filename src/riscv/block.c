#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "riscv/block.h"
#include "riscv/func.h"
#include "riscv/inst.h"
#include "riscv/reg.h"

RVBlock *newRVBlock(RVFunc *func, IRBlock *irBlock) {
  RVBlock *blk = calloc(1, sizeof(RVBlock));
  blk->func = func;
  blk->id = irBlock->id;

  blk->instHead = newRVInst(RV_ILLEGAL);
  blk->instTail = newRVInst(RV_ILLEGAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;

  return blk;
}

void printRVBlock(FILE *fp, RVBlock *block, bool debug) {
  fprintf(fp, ".%s.B%d:\n", block->func->name, block->id);

  if (debug) {
    fprintf(fp, "# preds =");
    for (int i = 0; i < arrlen(block->preds); i++)
      fprintf(fp, " .B%d", block->preds[i]->id);
    fprintf(fp, "\n# liveIns =");
    for (int i = 0; i < arrlen(block->liveIns); i++) {
      fprintf(fp, " ");
      printReg(fp, block->liveIns[i]);
    }
    fprintf(fp, "\n# gens =");
    for (int i = 0; i < arrlen(block->gens); i++) {
      fprintf(fp, " ");
      printReg(fp, block->gens[i]);
    }
    fprintf(fp, "\n# kills =");
    for (int i = 0; i < arrlen(block->kills); i++) {
      fprintf(fp, " ");
      printReg(fp, block->kills[i]);
    }
    fprintf(fp, "\n# liveOuts =");
    for (int i = 0; i < arrlen(block->liveOuts); i++) {
      fprintf(fp, " ");
      printReg(fp, block->liveOuts[i]);
    }
    fprintf(fp, "\n# succs =");
    for (int i = 0; i < arrlen(block->succs); i++)
      fprintf(fp, " .B%d", block->succs[i]->id);
    fprintf(fp, "\n");
  }

  for (RVInst *inst = block->instHead->next; inst != block->instTail;
       inst = inst->next) {
    printRVInst(fp, inst, debug);
  }
}
