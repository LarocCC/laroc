#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/block.h"
#include "ir/func.h"
#include "riscv/block.h"
#include "riscv/frameobj.h"
#include "riscv/func.h"

RVFunc *newRVFunc(IRFunc *irFunc) {
  RVFunc *func = calloc(1, sizeof(RVFunc));
  func->name = irFunc->name;

  for (int i = 0; i < arrlen(irFunc->allocas); i++)
    funcAddFrameObjectFromAlloca(func, irFunc->allocas[i]);

  func->blockCount = irFunc->blockCount;

  arrsetcap(func->blocks, irFunc->blockCount);
  arrput(func->blocks, NULL);
  for (int i = 1; i <= func->blockCount; i++)
    arrput(func->blocks, newRVBlock(func, irFunc->blocks[i]));

  func->entry = func->blocks[irFunc->entry->id];
  for (int i = 0; i < arrlen(irFunc->exits); i++)
    arrput(func->exits, func->blocks[irFunc->exits[i]->id]);

  return func;
}

void printRVFunc(RVFunc *func, bool debug) {
  printf("\t.globl\t%s\n", func->name);
  printf("\t.p2align\t1\n");
  printf("\t.type\t%s, @function\n", func->name);
  printf("%s:\n", func->name);

  if (debug) {
    printf("# entry = .B%d\n", func->entry->id);
    printf("# exits =");
    for (int i = 0; i < arrlen(func->exits); i++)
      printf(" .B%d", func->exits[i]->id);
    printf("\n");

    for (int i = 0; i < arrlen(func->frameObjs); i++)
      printFrameObject(func->frameObjs[i]);
  }

  for (int i = 1; i <= func->blockCount; i++)
    printRVBlock(func->blocks[i], debug);
}
