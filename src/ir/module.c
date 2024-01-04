#include <stdio.h>

#include "stb/stb_ds.h"

#include "ir/inst.h"
#include "ir/module.h"
#include "ir/type.h"
#include "ir/value.h"

void printModule(Module *mod) {
  for (int i = 0; i < arrlen(mod->funcs); i++) {
    if (i != 0)
      printf("\n");
    printIRFunc(mod->funcs[i]);
  }
}

IRFunc *newIRFunc(const char *name) {
  IRFunc *func = calloc(1, sizeof(IRFunc));
  func->name = name;
  arrput(func->blocks, NULL);
  return func;
}

void printIRFunc(IRFunc *func) {
  printf("func ");
  printIRType(func->ret);
  printf(" @%s(", func->name);
  for (int i = 0; i < arrlen(func->args); i++) {
    if (i != 0)
      printf(", ");
    printValue(func->args[i]);
  }
  printf(") {\n");

  printf("; entry = .B%d\n", func->entry->id);
  printf("; exits =");
  for (int i = 0; i < arrlen(func->exits); i++)
    printf(" .B%d", func->exits[i]->id);
  printf("\n");

  for (int i = 0; i < arrlen(func->allocas); i++)
    printIRInst(func->allocas[i], true);

  for (int i = 1; i <= func->blockCount; i++)
    printIRBlock(func->blocks[i]);

  printf("}\n");
}

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
