#include <stdio.h>

#include "stb/stb_ds.h"

#include "ir/block.h"
#include "ir/func.h"
#include "ir/inst.h"

IRBlock *newIRBlock(IRFunc *func) {
  IRBlock *blk = calloc(1, sizeof(IRBlock));
  arrput(func->blocks, blk);
  blk->id = ++func->blockCount;
  blk->instHead = newIRInst(IR_INVAL);
  blk->instTail = newIRInst(IR_INVAL);
  blk->instHead->next = blk->instTail;
  blk->instTail->prev = blk->instHead;
  return blk;
}

void printIRBlock(IRBlock *blk) {
  printf("\n.B%d:\n", blk->id);

  printf("; preds =");
  for (int i = 0; i < arrlen(blk->preds); i++)
    printf(" .B%d", blk->preds[i]->id);
  printf("\n; succs =");
  for (int i = 0; i < arrlen(blk->succs); i++)
    printf(" .B%d", blk->succs[i]->id);
  printf("\n");

  for (IRInst *inst = blk->instHead->next; inst != blk->instTail;
       inst = inst->next)
    printIRInst(inst, true);
}
